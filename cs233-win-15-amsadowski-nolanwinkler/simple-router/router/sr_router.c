/**********************************************************************
 * file:  sr_router.c
 * date:  Mon Feb 18 12:50:42 PST 2002
 * Contact: casado@stanford.edu
 *
 * Description:
 *
 * This file contains all the functions that interact directly
 * with the routing table, as well as the main entry method
 * for routing.
 *
 **********************************************************************/

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "sr_arpcache.h"
#include "sr_if.h"
#include "sr_rt.h"
#include "sr_router.h"
#include "sr_protocol.h"
#include "sr_arpcache.h"
#include "sr_utils.h"



/*---------------------------------------------------------------------
 * Method: sr_init(void)
 * Scope:  Global
 *
 * Initialize the routing subsystem
 *
 *---------------------------------------------------------------------*/

void sr_init(struct sr_instance* sr)
{
  /* REQUIRES */
  assert(sr);

  /* Initialize cache and cache cleanup thread */
  sr_arpcache_init(&(sr->cache));

  pthread_attr_init(&(sr->attr));
  pthread_attr_setdetachstate(&(sr->attr), PTHREAD_CREATE_JOINABLE);
  pthread_attr_setscope(&(sr->attr), PTHREAD_SCOPE_SYSTEM);
  pthread_attr_setscope(&(sr->attr), PTHREAD_SCOPE_SYSTEM);
  pthread_t thread;

  pthread_create(&thread, &(sr->attr), sr_arpcache_timeout, sr);
    
  /* Add initialization code here! */

} /* -- sr_init -- */

//forward declare
void sr_handle_ip_packet(struct sr_instance * sr,   uint8_t * ip_packet/* lent */, unsigned int len,   char* interface/* lent */);
void sr_handle_arp_packet(struct sr_instance * sr,   uint8_t * arp_packet/* lent */, unsigned int len,   char* interface/* lent */);
void sr_handle_arp_request(struct sr_instance * sr,   uint8_t * arp_request/* lent */, unsigned int len,   char* interface/* lent */);
void sr_handle_arp_reply(struct sr_instance * sr,   uint8_t * arp_reply/* lent */, unsigned int len,   char* interface/* lent */);

/*---------------------------------------------------------------------
 * Method: sr_handlepacket(uint8_t* p,char* interface)
 * Scope:  Global
 *
 * This method is called each time the router receives a packet on the
 * interface.  The packet buffer, the packet length and the receiving
 * interface are passed in as parameters. The packet is complete with
 * ethernet headers.
 *
 * Note: Both the packet buffer and the character's memory are handled
 * by sr_vns_comm.c that means do NOT delete either.  Make a copy of the
 * packet instead if you intend to keep it around beyond the scope of
 * the method call.
 *
 *---------------------------------------------------------------------*/

//takes in an ethernet frame/packet, i.e. first thing we look at is an ethernet header
void sr_handlepacket(struct sr_instance* sr,
		     uint8_t * packet/* lent */,
		     unsigned int len,
		     char* interface/* lent */)
{
  /* REQUIRES */
  assert(sr);
  assert(packet);
  assert(interface);

  printf("*** -> Received packet of length %d \n",len);

  /* fill in code here */

  printf("Got an ethernet frame, so printing out everything: ");
  print_hdrs(packet, len);


  //packet buffer contains full packet including ethernet header
  //DON'T FREE THE PACKET!

  uint16_t our_ethertype = ethertype(packet);

  if(our_ethertype == ethertype_ip) {
    sr_handle_ip_packet(sr, packet, len, interface);
  }

  else if(our_ethertype == ethertype_arp) {
    sr_handle_arp_packet(sr, packet, len, interface);
  }

  else
    perror("Hey, that's not an ethertype we expect in your packet!\n");

  
}/* end sr_ForwardPacket */

//finds the length of a mask by counting the 1's
//UNNECESSARY AND PROBABLY WRONG, we don't need longest length match any more as per piaza @420 (blaze it)
int mask_length(struct in_addr mask){
  int size = sizeof(unsigned long);
  int count, i;
  unsigned long mask_bits = mask.s_addr;

  i=0;
  while(i < size){
    count += mask_bits & 1;
    mask_bits = mask_bits >> 1;
    i++;
  }

  return count;
}

//looks to see if a given ip matches any of our interfaces, if it does, returns that interface
//if no matches, returns NULL
struct sr_if *find_if_match(struct sr_instance *sr, uint32_t dst_ip){
  struct sr_if *if_list = sr->if_list;
  while(if_list != NULL){
    if(if_list->ip == dst_ip) //match
      return if_list;
    if_list = if_list->next;
  }
  return NULL; //no match
}

//looks for a match in the lookup table
//if no matches, returns NULL
struct sr_rt *find_rt_match(struct sr_instance *sr, uint32_t dst_ip){
  struct sr_rt *rt = sr->routing_table;
  in_addr_t entryandmask, dstandmask;
  while(rt != NULL){
    entryandmask = rt->dest.s_addr & rt->mask.s_addr;
    dstandmask = (unsigned long)dst_ip & rt->mask.s_addr;
    if(entryandmask == dstandmask){ //match
      return rt;
    }
    rt = rt->next; //no match
  }
  return NULL;
}

/*remember packet and interface still lent */
void sr_handle_ip_packet(struct sr_instance * sr,   uint8_t * packet/* lent */, unsigned int len,   char* interface/* lent */)
{

  printf("We have an IP packet\n");  

  sr_ethernet_hdr_t *eth_hdr = (sr_ethernet_hdr_t *) packet;
  sr_ip_hdr_t * ip_hdr = (sr_ip_hdr_t *)(packet+sizeof(sr_ethernet_hdr_t));
  sr_icmp_hdr_t *icmp_header;

  uint16_t packet_cksum;
  uint16_t calc_cksum;
  uint8_t our_ip_protocol;

  //copy it so we can verify the checksum without possibly messing up the packet data itself
  //this might be an unnecessary precaution
  uint8_t * orig_packet_copy = malloc(len);
  memcpy(orig_packet_copy, packet, len);
  sr_ip_hdr_t * orig_cp_ip_hdr = (sr_ip_hdr_t *)(orig_packet_copy+sizeof(sr_ethernet_hdr_t));

  packet_cksum = orig_cp_ip_hdr->ip_sum;
  //have to 0 cksum field before calculating new cksum
  orig_cp_ip_hdr->ip_sum = 0;
  calc_cksum = cksum(orig_cp_ip_hdr, sizeof(sr_ip_hdr_t));
  our_ip_protocol = ip_hdr->ip_p;

  free(orig_packet_copy);
  
  //sanity check: right length
  if(len < 20 || len > 65535){
    printf("ERROR: packet unreasonable length\n");
    return;
  }

  //sanity check: checksum matches
  if(calc_cksum != packet_cksum){
    printf("ERROR: packet checksum does not match\n");
    printf("calculated was %d and packet was %d", calc_cksum, packet_cksum);
    return;
  }

  //check if destined for router or interfaces
  struct sr_if *if_match = find_if_match(sr, ip_hdr->ip_dst);

  if(if_match != NULL){ //it is going to one of our interfaces
    if(our_ip_protocol == ip_protocol_icmp){
      icmp_header = (sr_icmp_hdr_t *)(packet+sizeof(sr_ethernet_hdr_t)+sizeof(sr_ip_hdr_t));
      if(icmp_header->icmp_type == 8){ //echo
	//LETS TRY LITERALLY JUST SWAPPING VALUES
	icmp_header->icmp_type = 0;
	icmp_header->icmp_sum = 0;
	icmp_header->icmp_sum = cksum(packet+sizeof(sr_ethernet_hdr_t)+sizeof(sr_ip_hdr_t), sizeof(sr_icmp_hdr_t));
	uint32_t tmp = ip_hdr->ip_src; 
	ip_hdr->ip_src = ip_hdr->ip_dst;
	ip_hdr->ip_dst = tmp;
	ip_hdr->ip_sum = 0;
	ip_hdr->ip_sum = cksum(packet+sizeof(sr_ethernet_hdr_t), sizeof(sr_ip_hdr_t));
	memcpy(eth_hdr->ether_dhost,eth_hdr->ether_shost,6);
	memcpy(eth_hdr->ether_shost,sr_get_interface(sr,interface)->addr,6);
	sr_send_packet(sr, packet, len, interface);
      } else {
	//we don't process non-echo icmp requests to our ports
	return;
      }
    } else { //its not icmp_type
      //you fool, our ports cannot handle udp or tcp payloads
      //send back 3,3
      unsigned int *lennew = malloc(sizeof(unsigned int));
      uint8_t *icmp_pac = make_icmp_packet(sr, 3, 3, 0, (uint8_t *)ip_hdr, ICMP_DATA_SIZE, lennew);
      uint8_t *ip_pac = make_ip_packet(sr, 0, 64, ip_protocol_icmp, 0, sr_get_interface(sr,interface)->ip, ip_hdr->ip_src, icmp_pac, *lennew, lennew);
      uint8_t *eth_pac = make_eth_packet(sr, eth_hdr->ether_shost, sr_get_interface(sr,interface)->addr, ethertype_ip, ip_pac, *lennew, lennew);
      sr_send_packet(sr, eth_pac, *lennew, interface);
      free(lennew); free(icmp_pac); free(ip_pac); free(eth_pac);
    }
  } 
  else { //its not for us, so we must route it 

    //decrement time to live because we are gonna forward this packet
    ip_hdr->ip_ttl--;

    //if our time to live is up...
    /*         ...
             ;::::;
           ;::::; :;
         ;:::::'   :;
        ;:::::;     ;.
       ,:::::'       ;           OOO\
       ::::::;       ;          OOOOO\
       ;:::::;       ;         OOOOOOOO
      ,;::::::;     ;'         / OOOOOOO
    ;:::::::::`. ,,,;.        /  / DOOOOOO
  .';:::::::::::::::::;,     /  /     DOOOO
 ,::::::;::::::;;;;::::;,   /  /        DOOO
;`::::::`'::::::;;;::::: ,#/  /          DOOO
:`:::::::`;::::::;;::: ;::#  /            DOOO
::`:::::::`;:::::::: ;::::# /              DOO
`:`:::::::`;:::::: ;::::::#/               DOO
 :::`:::::::`;; ;:::::::::##                OO
 ::::`:::::::`;::::::::;:::#                OO
 `:::::`::::::::::::;'`:;::#                O
  `:::::`::::::::;' /  / `:#
   ::::::`:::::;'  /  /   `# */

    if(ip_hdr->ip_ttl <= 0){
      struct sr_if *src_if = sr_get_interface(sr, interface);
      unsigned int *lenttl = malloc(sizeof(unsigned int));
      uint8_t *icmp_pac = make_icmp_packet(sr, 11, 0, 0, (uint8_t *)ip_hdr, ICMP_DATA_SIZE, lenttl);
      uint8_t *ip_pac = make_ip_packet(sr, 0, 10, ip_protocol_icmp, ip_hdr->ip_id, src_if->ip, ip_hdr->ip_src, icmp_pac, *lenttl, lenttl);
      uint8_t *eth_pac = make_eth_packet(sr,  eth_hdr->ether_shost, src_if->addr, ethertype_ip, ip_pac, *lenttl, lenttl);
      sr_send_packet(sr, eth_pac, *lenttl, interface);
      //free the packet now that its sent
      free(lenttl); free(icmp_pac); free(ip_pac); free(eth_pac);
      return;
    }

    //ttl was changed so need to re-do checksum
    ip_hdr->ip_sum = 0;
    ip_hdr->ip_sum = cksum(ip_hdr, sizeof(sr_ip_hdr_t));

    //find longest prefix match in routing table
    struct sr_rt *rt = find_rt_match(sr, ip_hdr->ip_dst);

    if(rt != NULL){ //we have a match
      //check arp cache for next hop and send
      struct sr_if *send_if = sr_get_interface(sr,rt->interface);
      struct sr_arpentry *arpentry = sr_arpcache_lookup(&sr->cache, ip_hdr->ip_dst);
      if(arpentry != NULL){ //its already in our cache, we can just forward the packet
	memcpy(eth_hdr->ether_shost, send_if->addr, ETHER_ADDR_LEN);
	memcpy(eth_hdr->ether_dhost, arpentry->mac, ETHER_ADDR_LEN);
	sr_send_packet(sr, packet, len, rt->interface);
	free(arpentry);
      }
      else { //its not in our cache, lets send a request
	//want to send this ARP request out from the interface for the network closest to dest IP
	struct sr_if * relevant_IF = sr_get_interface(sr, rt->interface);
	struct sr_arpreq *request = sr_arpcache_queuereq(&sr->cache, ip_hdr->ip_dst, packet, len, relevant_IF->name);
	handle_cached_request(request, sr); //sends initial arp request immediately and does # sent stuff, etc.
      }
    } 
    else { //it wasn't in our routing table, so the destination is unreachable
      struct sr_if *src_if = sr_get_interface(sr, interface);
      unsigned int *lendu = malloc(sizeof(unsigned int));
      uint8_t *icmp_pac = make_icmp_packet(sr, 3, 0, 0, (uint8_t *)ip_hdr, ICMP_DATA_SIZE, lendu);
      uint8_t *ip_pac = make_ip_packet(sr, 0, 10, ip_protocol_icmp, ip_hdr->ip_id, src_if->ip, ip_hdr->ip_src, icmp_pac, *lendu, lendu);
      uint8_t *eth_pac = make_eth_packet(sr, eth_hdr->ether_shost, src_if->addr, ethertype_ip, ip_pac, *lendu, lendu);  
      sr_send_packet(sr, eth_pac, (unsigned int)*lendu, src_if->name);
      free(lendu); free(icmp_pac); free(ip_pac); free(eth_pac);
    }
  }
}

//while packet is an ethernet packet, we can get our the arp header
void sr_handle_arp_packet(struct sr_instance * sr,   uint8_t * arp_packet/* lent */, unsigned int len,   char* interface/* lent */)
{
  sr_arp_hdr_t * arp_hdr = (sr_arp_hdr_t *)(arp_packet+sizeof(sr_ethernet_hdr_t));

  //check if reply or request
  if(ntohs(arp_hdr->ar_op) == arp_op_reply) { 
    sr_handle_arp_reply(sr, arp_packet, len, interface);
  }
  else if(ntohs(arp_hdr->ar_op) == arp_op_request) {
    sr_handle_arp_request(sr, arp_packet, len, interface);
  }
  else
    perror("Hey, arp packets are supposed to be partitioned into replies and requests!\n"); 
  
}

void sr_handle_arp_reply(struct sr_instance * sr,   uint8_t * arp_reply/* lent */, unsigned int len,   char* interface/* lent */)
{
  sr_arp_hdr_t * arp_hdr = (sr_arp_hdr_t *)(arp_reply+sizeof(sr_ethernet_hdr_t));
  struct sr_if *if_match = find_if_match(sr, arp_hdr->ar_tip);

  if(if_match != NULL) {       //store our interface's MAC adress <-> packet's src IP address pairing in the cache if it's not yet there
    struct sr_arpreq * is_there = sr_arpcache_insert(&sr->cache, if_match->addr, arp_hdr->ar_sip);
    if(is_there!=0) { //we had pending packets waiting on this ARP reply which we send
      struct sr_packet * depending_packets = is_there->packets;
      struct sr_packet * current_packet = depending_packets;
      while(current_packet!=NULL) {
	sr_ethernet_hdr_t * sending_hdr = (sr_ethernet_hdr_t *)(current_packet->buf);
	memcpy(sending_hdr->ether_shost, if_match->addr, (sizeof(unsigned char)*ETHER_ADDR_LEN)); //give them src mac address of the interface
	memcpy(sending_hdr->ether_dhost, arp_hdr->ar_sha, (sizeof(unsigned char)*ETHER_ADDR_LEN)); //give them dest mac address of the thing that sent us it
	sr_send_packet(sr, current_packet->buf, current_packet->len, interface);
	current_packet = current_packet->next;
      }
      sr_arpreq_destroy(&sr->cache, is_there);
    }
  }

}

//make and send ARP reply for interface we got the request on
void sr_handle_arp_request(struct sr_instance * sr,   uint8_t * arp_request/* lent */, unsigned int len,   char* interface/* lent */)
{
  printf("This ARP Packet is an ARP request\n");
  sr_ethernet_hdr_t * eth_hdr = (sr_ethernet_hdr_t *)arp_request;
  sr_arp_hdr_t * arp_hdr = (sr_arp_hdr_t *)(arp_request+sizeof(sr_ethernet_hdr_t));

  struct sr_if *if_match = find_if_match(sr, arp_hdr->ar_tip);

  if(if_match != NULL) {
    unsigned int *lennew = malloc(sizeof(unsigned int));
    uint8_t * arp_pac = make_arp_packet(sr, arp_op_reply, if_match->addr, if_match->ip, eth_hdr->ether_shost, arp_hdr->ar_sip);
    uint8_t * response_packet = make_eth_packet(sr, eth_hdr->ether_shost, if_match->addr, ethertype_arp, arp_pac, sizeof(sr_arp_hdr_t), lennew);

    int response_len = sizeof(sr_ethernet_hdr_t) + sizeof(sr_arp_hdr_t);

    sr_send_packet(sr, response_packet, response_len, interface);
    free(response_packet);
  }

}


uint8_t * make_icmp_packet(struct sr_instance * sr, uint8_t type, uint8_t code, uint16_t next_mtu, uint8_t *data, unsigned int len, unsigned int *returnlen)
{  
  if(type != 3 && type != 11) {
    int headersize = sizeof(sr_icmp_hdr_t);
    sr_icmp_hdr_t *header = calloc(1,headersize);

    header->icmp_type = type;
    header->icmp_code = code;
    header->icmp_sum = 0;
    
    //now update checksum
    header->icmp_sum = cksum(header, headersize);

    //add in actual packet
    uint8_t *newpacket = calloc(1, headersize + len*sizeof(uint8_t));
    memcpy(newpacket, header, headersize);
    memcpy(newpacket+headersize, data, len);

    *returnlen = headersize + len; 

    return newpacket;
    
  } else {
    int headersize = sizeof(sr_icmp_t3_hdr_t);
    sr_icmp_t3_hdr_t *header = calloc(1,headersize);

    header->icmp_type = type;
    header->icmp_code = code;
    header->icmp_sum = 0;
    header->unused = 0;

    //copy over data
    sr_ip_hdr_t *iphdr = (sr_ip_hdr_t *)data;
    iphdr->ip_sum = 0;
    iphdr->ip_sum = cksum(iphdr, sizeof(sr_ip_hdr_t));

    memcpy(header->data, data, len);
    
    //now update checksum
    header->icmp_sum = cksum(header, headersize);
    *returnlen = headersize;

    return (uint8_t *)header;
  }
}

uint8_t *make_arp_packet(struct sr_instance *sr, unsigned short ar_op, unsigned char *ar_sha, uint32_t ar_sip, unsigned char *ar_tha, uint32_t ar_tip){
  sr_arp_hdr_t *header = calloc(1, sizeof(sr_arp_hdr_t));
  header->ar_hrd = htons(arp_hrd_ethernet);
  header->ar_pro = htons(ethertype_ip);
  header->ar_hln = (unsigned char)ETHER_ADDR_LEN;
  header->ar_pln = (unsigned char)sizeof(uint32_t);
  header->ar_op = htons(ar_op);
  header->ar_sip = ar_sip;
  header->ar_tip = ar_tip;
  memcpy(header->ar_sha,ar_sha, ETHER_ADDR_LEN * sizeof(unsigned char));
  memcpy(header->ar_tha,ar_tha, ETHER_ADDR_LEN * sizeof(unsigned char));

  return (uint8_t *) header;
}

uint8_t *make_ip_packet(struct sr_instance * sr, uint8_t ip_tos, uint8_t ip_ttl, uint8_t ip_p, uint16_t ip_id, uint32_t ip_src, uint32_t ip_dst, uint8_t *data, unsigned int len, unsigned int *returnlen){
  int headersize = sizeof(sr_ip_hdr_t);
  sr_ip_hdr_t * header = calloc(1,headersize);
  header->ip_v = 4;
  header->ip_hl = 5;
  header->ip_tos = ip_tos;
  header->ip_ttl = ip_ttl;
  header->ip_p = ip_p;
  header->ip_id = ip_id;
  header->ip_src = ip_src;
  header->ip_dst = ip_dst;
  header->ip_len = htons(headersize + len);
  header->ip_sum = 0;
  header->ip_sum = cksum(header, headersize);

  uint8_t *newpacket = calloc(1,headersize + len*sizeof(uint8_t));
  memcpy(newpacket, header, headersize);
  memcpy(newpacket+headersize, data, len);
  
  *returnlen = headersize + len*sizeof(uint8_t);

  free(header);

  return newpacket;
}

uint8_t *make_eth_packet(struct sr_instance * sr, uint8_t *dest, uint8_t *src, uint16_t type, uint8_t *data, unsigned int len, unsigned int *returnlen){
  int headersize = sizeof(sr_ethernet_hdr_t);
  sr_ethernet_hdr_t *header = calloc(1,headersize);
  memcpy(header->ether_dhost, dest, ETHER_ADDR_LEN);
  memcpy(header->ether_shost, src, ETHER_ADDR_LEN);
  header->ether_type = htons(type);

  uint8_t *newpacket = calloc(1,headersize + len*sizeof(uint8_t));

  memcpy(newpacket, header, headersize);
  memcpy(newpacket+headersize, data, len);

  *returnlen = headersize + len*sizeof(uint8_t);

  free(header);

  return newpacket;
}





