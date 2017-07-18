#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>
#include "sr_arpcache.h"
#include "sr_router.h"
#include "sr_if.h"
#include "sr_protocol.h"
#include "sr_rt.h"
#include "sr_utils.h"


/* Helper function to sr_arpcache_sweepreqs */
/* Takes in a cached request and sees whether it has been sent 5 times or not, whether it needs to be resent, and how to respond to the packets waiting on this request based off of that */
void handle_cached_request(struct sr_arpreq * req, struct sr_instance *sr) {
  printf("entered into our inner cache function\n");     
  time_t now;
  struct sr_packet *cur;
  struct sr_if *ifc;
  struct sr_rt * clients_rt;
  sr_ethernet_hdr_t *eth_hdr;
  sr_ip_hdr_t * ip_hdr;
  now = time(NULL);
  if(difftime(now, req->sent) > 1.0) {
    printf("we called our sweep req function on a req!\n");
    if(req->times_sent >= 5) {
      printf("Times sent >= 5\n");
      //send icmp host unreachable to source addr of all pkts waiting on this request
      cur = req->packets;
      while(cur != NULL){
	eth_hdr = (sr_ethernet_hdr_t *) cur->buf;
	ip_hdr = (sr_ip_hdr_t *)(cur->buf+sizeof(sr_ethernet_hdr_t));

        clients_rt = find_rt_match(sr, ip_hdr->ip_src); //interface packet was sent in to us
	ifc = sr_get_interface(sr, clients_rt->interface);
	unsigned int *len = malloc(sizeof(unsigned int));
	uint8_t *icmp_pac = make_icmp_packet(sr, 3, 1, 0, cur->buf, ICMP_DATA_SIZE, len);
	uint8_t *ip_pac = make_ip_packet(sr, 0, 10, ip_protocol_icmp, ip_hdr->ip_id, ifc->ip, ip_hdr->ip_src, icmp_pac, *len, len);
	uint8_t *eth_pac = make_eth_packet(sr, eth_hdr->ether_shost, ifc->addr, ethertype_arp, ip_pac, *len, len);

        printf("Sending off ICMP host unreachables to all of the hosts that sent us packets for IPs we can't find");
        printf("value of len is %d", *len);  
        print_hdrs(eth_pac, (unsigned int)*len);
	sr_send_packet(sr, eth_pac, (unsigned int)*len, ifc->name);

	//free the packet now that its sent
	free(len); free(icmp_pac); free(ip_pac); free(eth_pac);
        cur = cur->next;
        printf("looping on while cur != NULL\n");
      }
      printf("freeing this request\n"); //because done with it
      sr_arpreq_destroy(&sr->cache, req);
    }
    else{
      printf("Times sent <5, so sending arp req\n");
      //(re)send arp request by constructing one from the info in the packets and sending it out to target IP address

                
      //get out a packet waiting on this arp request
      printf("above getting a packet out\n");
      struct sr_packet * a_packet = req->packets;
      if(a_packet==NULL) {
	req->sent = now;
	req->times_sent = 5; //slightly janky, we admit, but this should never happen so we just ignore this request.
	return;
      }
      printf("below getting a packet out\n");
      sr_ethernet_hdr_t * packet_eth_hdr = (sr_ethernet_hdr_t *)(a_packet->buf);
      if(packet_eth_hdr->ether_type != ethertype_ip) {
	printf("Ruh-Roh, ether_type is %d\n", packet_eth_hdr->ether_type);
      }
      printf("above gettin IP header out\n");
      sr_ip_hdr_t * packet_ip_hdr = (sr_ip_hdr_t *)(a_packet->buf+sizeof(sr_ethernet_hdr_t));          
      /*copy info from this packet into the arp request

      //sending this packet from the router's interface corresponding to the dest IP's network
      //broadcast MAC address
      //sending out from relevant interface - did prefix matching when put it into the queue
      //trying to find MAC corresponding to the packet's dest IP */
      
      printf("below IP header, above get if\n");         
      struct sr_if * relevant_IF = sr_get_interface(sr, req->packets->iface);
      printf("below get if, above making packets\n");
      unsigned char zeroes[ETHER_ADDR_LEN];
      memset(zeroes, 0, ETHER_ADDR_LEN);
      unsigned char broadcast[ETHER_ADDR_LEN];
      memset(broadcast, 0xff, (sizeof(unsigned char)*ETHER_ADDR_LEN));
      uint8_t * arp_arp = make_arp_packet(sr, arp_op_request, relevant_IF->addr, relevant_IF->ip, zeroes, packet_ip_hdr->ip_dst);
      unsigned int * returnlen = malloc(sizeof(unsigned int));
      uint8_t * arp_eth = make_eth_packet(sr, broadcast, relevant_IF->addr, ethertype_arp, arp_arp, sizeof(sr_arp_hdr_t), returnlen);

      //when we have a full packet, send it off
      printf("below making packets, Sending off an ARP request\n");
      sr_send_packet(sr, arp_eth, *returnlen, relevant_IF->name);
      printf("below sending packet, above freeing\n");
      free(returnlen); free(arp_arp); free(arp_eth);
      printf("below freeing\n");

      req->sent = now;
      req->times_sent++;
    }
  }
  else {
    printf("the time difference was less than 1 for this struct, so didnt do anything\n");
  }

}


/* 
   This function gets called every second. For each request sent out, we keep
   checking whether we should resend an request or destroy the arp request.
   See the comments in the header file for an idea of what it should look like.
*/
//iterate through the ARP request queue, re-sending any outstanding ARP requests not sent in the past second
void sr_arpcache_sweepreqs(struct sr_instance *sr) { 
  printf("Entered our sweep reqs outer function\n");
  struct sr_arpreq * request_q = sr->cache.requests; /* ARP Request Queue */
  struct sr_arpreq * curr_req = request_q;
  while(curr_req != NULL) { //save next pointer because cached could delte curr req
    struct sr_arpreq * next_req = curr_req->next;
    handle_cached_request(curr_req, sr); //does all real functionality - see above
    curr_req = next_req;
    printf("looping in curr_req != NULL\n");
  }
}

/* You should not need to touch the rest of this code. */

/* Checks if an IP->MAC mapping is in the cache. IP is in network byte order.
   You must free the returned structure if it is not NULL. */
struct sr_arpentry *sr_arpcache_lookup(struct sr_arpcache *cache, uint32_t ip) {
  pthread_mutex_lock(&(cache->lock));
    
  struct sr_arpentry *entry = NULL, *copy = NULL;
    
  int i;
  for (i = 0; i < SR_ARPCACHE_SZ; i++) {
    if ((cache->entries[i].valid) && (cache->entries[i].ip == ip)) {
      entry = &(cache->entries[i]);
    }
  }
    
  /* Must return a copy b/c another thread could jump in and modify
     table after we return. */
  if (entry) {
    copy = (struct sr_arpentry *) malloc(sizeof(struct sr_arpentry));
    memcpy(copy, entry, sizeof(struct sr_arpentry));
  }
        
  pthread_mutex_unlock(&(cache->lock));
    
  return copy;
}

/* Adds an ARP request to the ARP request queue. If the request is already on
   the queue, adds the packet to the linked list of packets for this sr_arpreq
   that corresponds to this ARP request. You should free the passed *packet.
   
   A pointer to the ARP request is returned; it should not be freed. The caller
   can remove the ARP request from the queue by calling sr_arpreq_destroy. */
struct sr_arpreq *sr_arpcache_queuereq(struct sr_arpcache *cache,
                                       uint32_t ip,
                                       uint8_t *packet,           /* borrowed */
                                       unsigned int packet_len,
                                       char *iface)
{
  pthread_mutex_lock(&(cache->lock));
    
  struct sr_arpreq *req;
  for (req = cache->requests; req != NULL; req = req->next) {
    if (req->ip == ip) {
      break;
    }
  }
    
  /* If the IP wasn't found, add it */
  if (!req) {
    req = (struct sr_arpreq *) calloc(1, sizeof(struct sr_arpreq));
    req->ip = ip;
    req->next = cache->requests;
    cache->requests = req;
  }
    
  /* Add the packet to the list of packets for this request */
  if (packet && packet_len && iface) {
    struct sr_packet *new_pkt = (struct sr_packet *)malloc(sizeof(struct sr_packet));
        
    new_pkt->buf = (uint8_t *)malloc(packet_len);
    memcpy(new_pkt->buf, packet, packet_len);
    new_pkt->len = packet_len;
    new_pkt->iface = (char *)malloc(sr_IFACE_NAMELEN);
    strncpy(new_pkt->iface, iface, sr_IFACE_NAMELEN);
    new_pkt->next = req->packets;
    req->packets = new_pkt;
  }
    
  pthread_mutex_unlock(&(cache->lock));
    
  return req;
}

/* This method performs two functions:
   1) Looks up this IP in the request queue. If it is found, returns a pointer
   to the sr_arpreq with this IP. Otherwise, returns NULL.
   2) Inserts this IP to MAC mapping in the cache, and marks it valid. */
struct sr_arpreq *sr_arpcache_insert(struct sr_arpcache *cache,
                                     unsigned char *mac,
                                     uint32_t ip)
{
  pthread_mutex_lock(&(cache->lock));
    
  struct sr_arpreq *req, *prev = NULL, *next = NULL; 
  for (req = cache->requests; req != NULL; req = req->next) {
    if (req->ip == ip) {            
      if (prev) {
	next = req->next;
	prev->next = next;
      } 
      else {
	next = req->next;
	cache->requests = next;
      }
            
      break;
    }
    prev = req;
  }
    
  int i;
  for (i = 0; i < SR_ARPCACHE_SZ; i++) {
    if (!(cache->entries[i].valid))
      break;
  }
    
  if (i != SR_ARPCACHE_SZ) {
    memcpy(cache->entries[i].mac, mac, 6);
    cache->entries[i].ip = ip;
    cache->entries[i].added = time(NULL);
    cache->entries[i].valid = 1;
  }
    
  pthread_mutex_unlock(&(cache->lock));
    
  return req;
}

/* Frees all memory associated with this arp request entry. If this arp request
   entry is on the arp request queue, it is removed from the queue. */
void sr_arpreq_destroy(struct sr_arpcache *cache, struct sr_arpreq *entry) {
  pthread_mutex_lock(&(cache->lock));
    
  if (entry) {
    struct sr_arpreq *req, *prev = NULL, *next = NULL; 
    for (req = cache->requests; req != NULL; req = req->next) {
      if (req == entry) {                
	if (prev) {
	  next = req->next;
	  prev->next = next;
	} 
	else {
	  next = req->next;
	  cache->requests = next;
	}
                
	break;
      }
      prev = req;
    }
        
    struct sr_packet *pkt, *nxt;
        
    for (pkt = entry->packets; pkt; pkt = nxt) {
      nxt = pkt->next;
      if (pkt->buf)
	free(pkt->buf);
      if (pkt->iface)
	free(pkt->iface);
      free(pkt);
    }
        
    free(entry);
  }
    
  pthread_mutex_unlock(&(cache->lock));
}

/* Prints out the ARP table. */
void sr_arpcache_dump(struct sr_arpcache *cache) {
  fprintf(stderr, "\nMAC            IP         ADDED                      VALID\n");
  fprintf(stderr, "-----------------------------------------------------------\n");
    
  int i;
  for (i = 0; i < SR_ARPCACHE_SZ; i++) {
    struct sr_arpentry *cur = &(cache->entries[i]);
    unsigned char *mac = cur->mac;
    fprintf(stderr, "%.1x%.1x%.1x%.1x%.1x%.1x   %.8x   %.24s   %d\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], ntohl(cur->ip), ctime(&(cur->added)), cur->valid);
  }
    
  fprintf(stderr, "\n");
}

/* Initialize table + table lock. Returns 0 on success. */
int sr_arpcache_init(struct sr_arpcache *cache) {  
  /* Seed RNG to kick out a random entry if all entries full. */
  srand(time(NULL));
    
  /* Invalidate all entries */
  memset(cache->entries, 0, sizeof(cache->entries));
  cache->requests = NULL;
    
  /* Acquire mutex lock */
  pthread_mutexattr_init(&(cache->attr));
  pthread_mutexattr_settype(&(cache->attr), PTHREAD_MUTEX_RECURSIVE);
  int success = pthread_mutex_init(&(cache->lock), &(cache->attr));
    
  return success;
}

/* Destroys table + table lock. Returns 0 on success. */
int sr_arpcache_destroy(struct sr_arpcache *cache) {
  return pthread_mutex_destroy(&(cache->lock)) && pthread_mutexattr_destroy(&(cache->attr));
}

/* Thread which sweeps through the cache and invalidates entries that were added
   more than SR_ARPCACHE_TO seconds ago. */
void *sr_arpcache_timeout(void *sr_ptr) {
  struct sr_instance *sr = sr_ptr;
  struct sr_arpcache *cache = &(sr->cache);
    
  while (1) {
    sleep(1.0);
        
    pthread_mutex_lock(&(cache->lock));
    
    time_t curtime = time(NULL);
        
    int i;    
    for (i = 0; i < SR_ARPCACHE_SZ; i++) {
      if ((cache->entries[i].valid) && (difftime(curtime,cache->entries[i].added) > SR_ARPCACHE_TO)) {
	cache->entries[i].valid = 0;
      }
    }
        
    sr_arpcache_sweepreqs(sr);

    pthread_mutex_unlock(&(cache->lock));
  }
    
  return NULL;
}

