/*
 * chiTCP - A simple, testable TCP stack
 *
 * Implementation of the TCP protocol.
 *
 * chiTCP follows a state machine approach to implementing TCP.
 * This means that there is a handler function for each of
 * the TCP states (CLOSED, LISTEN, SYN_RCVD, etc.). If an
 * event (e.g., a packet arrives) while the connection is
 * in a specific state (e.g., ESTABLISHED), then the handler
 * function for that state is called, along with information
 * about the event that just happened.
 *
 * Each handler function has the following prototype:
 *
 * int f(serverinfo_t *si, chisocketentry_t *entry, tcp_event_type_t event);
 *
 * si is a pointer to the chiTCP server info. The functions in
 * this file will not have to access the data in the server info,
 * but this pointer is needed to call other functions.
 *
 * entry is a pointer to the socket entry for the connection that
 * is being handled. The socket entry contains the actual TCP
 * data (variables, buffers, etc.), which can be extracted
 * like this:
 *
 * tcp_data_t *tcp_data = &entry->socket_state.active.tcp_data;
 *
 * Other than that, no other fields in "entry" should be read
 * or modified.
 *
 * event is the event that has caused the TCP thread to wake up. The
 * list of possible events corresponds roughly to the ones
 * specified in http://tools.ietf.org/html/rfc793#section-3.9.
 * They are:
 *
 * APPLICATION_CONNECT: Application has called socket_connect()
 * and a three-way handshake must be initiated.
 *
 * APPLICATION_SEND: Application has called socket_send() and
 * there is unsent data in the send buffer.
 *
 * APPLICATION_RECEIVE: Application has called socket_recv() and
 * any received-and-acked data in the recv buffer will be
 * collected by the application (up to the maximum specified
 * when calling socket_recv).
 *
 * APPLICATION_CLOSE: Application has called socket_close() and
 * a connection tear-down should be initiated.
 *
 * PACKET_ARRIVAL: A packet has arrived through the network and
 * needs to be processed (RFC 793 calls this "SEGMENT ARRIVES")
 *
 * TIMEOUT: A timeout (e.g., a retransmission timeout) has
 * happened.
 *
 */
/*
 * Copyright (c) 2013-2014, The University of Chicago
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or withsend
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * - Neither the name of The University of Chicago nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software withsend specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY send OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
/* Nolan Winkler and Aidan Sadowski, CMSC 233's implementation */
#include "chitcp/log.h"
#include "chitcp/buffer.h"
#include "serverinfo.h"
#include "connection.h"
#include "tcp.h"
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

/*Consider uncommented chilogs to be comments */

/* Project 2b stuff */

typedef struct reqmember_t {
  tcp_packet_t *packet;
  pthread_cond_t acked;
  pthread_mutex_t lock4ack;
  struct timespec sent;
  struct timespec timeout;
} reqmember_t;

typedef struct args_for_timeout_thread {
  reqmember_t * rtq_struct;
  serverinfo_t * si;
  chisocketentry_t * entry;
} to_args_t;

reqmember_t * make_reqmember(serverinfo_t * si, chisocketentry_t * entry, tcp_packet_t *packet);
void process_q_after_ack(serverinfo_t * si, chisocketentry_t * entry); //manages RTQ when we get an ack in, removing acked packets
void free_reqmember(reqmember_t *member);
void process_timeout(serverinfo_t * si, chisocketentry_t * entry); // manages RTQ when we get a time-out, going back and 'sending N'
void tcp_data_init(tcp_data_t *tcp_data);
void tcp_data_free(tcp_data_t *tcp_data);

/* Function for the timeout thread */
void * timeout_thread_function(void * args) {
  pthread_detach(pthread_self());

  //get out the args, as we do
  to_args_t * input = (to_args_t *)args;
  reqmember_t * thestruct;
  thestruct = input->rtq_struct;
  pthread_mutex_lock(&thestruct->lock4ack);
  chilog(WARNING, "started a timer");


  int ret = pthread_cond_timedwait(&thestruct->acked, &thestruct->lock4ack, &thestruct->timeout);
  if(ret == ETIMEDOUT) { //we timed out
    chilog(WARNING, "signaled a TIMEOUT event");
    chitcpd_timeout(input->si, input->entry);
  }
  else { //i.e. we got signaled b/c our packet was ACKed before RTT
    chilog(WARNING, "the packet for this thread got acked");
    //function processing the Q frees the struct so dont do it here
  }

  //chilog(WARNING, "leaving timeout thread");
  pthread_mutex_unlock(&thestruct->lock4ack);
  //chilog(WARNING, "unlocked acklock");
  pthread_exit(NULL);
}


/*Given a packet, creates a struct to put on the retransmission queue for that packet, appends it, and launches a timeout thread */
int chitcpd_tcp_add_packet_to_retrans_q(tcp_packet_t * packet_to_send, serverinfo_t * si, chisocketentry_t * entry) { //have to call this on every packet that's not just an ack
  chilog(WARNING, "adding a packet to the retransmission queue");
  tcp_data_t * our_data = &entry->socket_state.active.tcp_data;
  reqmember_t * retq_struct_for_packet = make_reqmember(si, entry, packet_to_send);
  //chilog(WARNING,"adding req to the queue");
  pthread_mutex_lock(&our_data->lock_retrans_q);
  //chilog(WARNING,"locked queue");
  list_append(&our_data->retrans_q, retq_struct_for_packet);
  chilog(WARNING,"added to queue");
  pthread_mutex_unlock(&our_data->lock_retrans_q);
  //chilog(WARNING, "unlocked queue");
  //chilog(WARNING, "making input struct");
  pthread_t timeout_thread;
  to_args_t * input = malloc(sizeof(to_args_t));
  input->rtq_struct = retq_struct_for_packet;
  input->si = si;
  input->entry = entry;
  chilog(WARNING, "making timeout thread for this packet");
  if (pthread_create(&timeout_thread, NULL, timeout_thread_function, ((void*)input) ) < 0)
    {
      chilog(DEBUG,"Could not create timeout thread");
      exit(-1);
    }
  //don't need to join/reap because detached
  return 1;
}


//adds two timespecs together
//adapted from http://codereview.stackexchange.com/questions/40176/correctness-of-calculations-with-struct-timespec
struct timespec timespec_add(struct timespec t1, struct timespec t2){
  int sec = t1.tv_sec + t2.tv_sec;
  int nsec = t1.tv_nsec + t2.tv_nsec;
  if(nsec >= 1000000000){
    nsec -= 1000000000;
    sec++;
  }
  return (struct timespec){.tv_sec = sec, .tv_nsec = nsec};
}

//packet -> retransmission queue struct including the packet
reqmember_t * make_reqmember(serverinfo_t * si, chisocketentry_t * entry, tcp_packet_t *packet){
  //chilog(WARNING, "making reqmember");
  tcp_data_t * tcp_data = &entry->socket_state.active.tcp_data;
  struct timespec rto = tcp_data->rto;
  
  //chilog(WARNING, "mallocing space");
  reqmember_t *member = malloc(sizeof(reqmember_t));
  member->packet = packet;

  //chilog(WARNING, "getting time inforamtion");
  struct timespec curtime;
  clock_gettime(CLOCK_REALTIME, &curtime);
  member->sent = curtime;
  member->timeout = timespec_add(curtime, rto);

  //chilog(WARNING,"initializing vairables");
  pthread_mutex_init(&member->lock4ack, NULL);
  pthread_cond_init(&member->acked, NULL);
  //chilog(WARNING,"done in make reqmember function");
  return member;
}

//returns:
// 1  if t1 > t2
// 0  if t1 == t2
// -1 if t1 < t2
int timespec_comp(struct timespec t1, struct timespec t2){
  if((int)t1.tv_sec == (int)t2.tv_sec){
    if((int)t1.tv_nsec == (int)t2.tv_nsec)
      return 0;
    if((int)t1.tv_nsec > (int)t2.tv_nsec)
      return -1;
    return 1;
  }
  if((int)t1.tv_sec < (int)t2.tv_sec)
    return -1;
  return 1;				
}

//return the smaller of the two timespecs
struct timespec timespec_min(struct timespec t1, struct timespec t2){
  if(timespec_comp(t1,t2))
    return t1;
  return t2;
}

struct timespec timespec_max(struct timespec t1, struct timespec t2){
  if(timespec_comp(t1, t2))
    return t2;
  return t1;
}

struct timespec timespec_sub(struct timespec t1, struct timespec t2){
  int nsec = t1.tv_nsec - t2.tv_nsec;
  int sec = t1.tv_sec - t2.tv_sec;
  if(nsec < 0){
    sec--;
    nsec += 1000000000;
  }
  return (struct timespec){.tv_sec = sec, .tv_nsec = nsec};
}

struct timespec timespec_scalar_mult(struct timespec t, double alpha){
  int sec = (int) t.tv_sec * 1000000000;
  int nsec = (int) (t.tv_nsec + sec) * alpha;
  sec = 0;
  while(nsec >= 1000000000){
    nsec -= 1000000000;
    sec++;
  }
  return (struct timespec){.tv_sec = sec, .tv_nsec = nsec};
}

//takes the most recent rtt and uses it to calculate the new rto
void update_rto(serverinfo_t *si, chisocketentry_t *entry, struct timespec recentrtt){
  tcp_data_t *tcp_data = &entry->socket_state.active.tcp_data;
  struct timespec srtt = tcp_data->srtt;
  srtt = timespec_scalar_mult(srtt, tcp_data->alpha);
  //chilog(WARNING, "srtt after mult is sec = %d, nsec = %d",  srtt.tv_sec, srtt.tv_nsec);
  srtt = timespec_add(srtt, timespec_scalar_mult(recentrtt,(1.0-tcp_data->alpha)));
  //chilog(WARNING, "srtt after add is sec = %d, nsec = %d",  srtt.tv_sec, srtt.tv_nsec);
  srtt = timespec_scalar_mult(srtt, tcp_data->beta);
  //chilog(WARNING, "srtt after second mult is sec = %d, nsec = %d",  srtt.tv_sec, srtt.tv_nsec);
  struct timespec rto = timespec_min(tcp_data->upper, timespec_max(srtt,tcp_data->lower));
  tcp_data->srtt = (struct timespec){.tv_sec=srtt.tv_sec, .tv_nsec=srtt.tv_nsec};
  tcp_data->rto = (struct timespec){.tv_sec=rto.tv_sec, .tv_nsec=rto.tv_nsec};
  chilog(WARNING, "updated srtt to be sec = %d, nsec = %d", srtt.tv_sec, srtt.tv_nsec);
  chilog(WARNING, "updated rto to be sec = %d, nsec = %d", rto.tv_sec, rto.tv_nsec);
}

void free_reqmember(reqmember_t *member){
  //chilog(WARNING, "freeing whole reqmember struct");
  chitcp_tcp_packet_free(member->packet);
  pthread_mutex_destroy(&member->lock4ack);
  pthread_cond_destroy(&member->acked);
  free(member);
}


//takes a reqmember and says whether the current time is after the timeout time
// 1 indicates timed out
// 0 indicates didn't time out
int timed_out(reqmember_t *member){
  struct timespec curtime;
  struct timespec membertime = member->timeout;
  int retval;
  clock_gettime(CLOCK_REALTIME, &curtime);
  if(curtime.tv_sec == membertime.tv_sec) {
    retval = curtime.tv_nsec > membertime.tv_nsec;
  }
  else {
    retval = curtime.tv_sec > membertime.tv_sec;
  }
  chilog(WARNING, "timed_out value for packet %d was %d", SEG_SEQ(member->packet),retval);
  return retval;
}


/* Resends all timed out packets and resets their information
   Go though queue twice:
   First time: Get the lowest sequence number of all packets that have timed out
   Second time: Resend every packet that has a seq number equal to or greater than
   the lowest seq that timed out, thus implementing go-back-N */
void process_timeout(serverinfo_t * si, chisocketentry_t * entry){
  chilog(WARNING, "processing timeout event by going through queue");
  pthread_mutex_t *lock_retrans_q;
  list_t *retrans_q;
  list_t to_req;
  reqmember_t *cur;
  int i,j,deleted,lsize,ndel;
  int * to_delete;
  int lowestseq = 0;
  tcp_packet_t *curpacket;
  tcp_data_t * tcp_data;
  list_init(&to_req);
  tcp_data = &entry->socket_state.active.tcp_data;
  retrans_q = &tcp_data->retrans_q;
  lock_retrans_q = &tcp_data->lock_retrans_q;
  pthread_mutex_lock(lock_retrans_q);

  //marks which elements need to be deleted from the queue - all the ones we are retransmitting
  j = 0;
  i = 0;
  ndel = 0;
  lsize = list_size(retrans_q);
  to_delete = calloc(sizeof(int)*lsize, 0);

  //iterate through queue - first pass
  //find lowest seq number that has timed out
  if(0 == list_iterator_start(retrans_q)){
    chilog(DEBUG, "Retransq could not be iterated - first pass.");
    return;
  }
  while(list_iterator_hasnext(retrans_q)){
    cur = list_iterator_next(retrans_q);
    curpacket = cur->packet;
    //find the lowest seq number that has timed out
    if(timed_out(cur)){
      //if this is the first element we are looking at, set initial value of lowestseq
      if(i == 0)
	lowestseq = SEG_SEQ(curpacket);
      if(SEG_SEQ(curpacket) < lowestseq)
	lowestseq = SEG_SEQ(curpacket);
    }
    i++;
  }
  list_iterator_stop(retrans_q);

  //if lowestseq is 0, no packets actually timed out, so how did we get in this function?
  if(lowestseq == 0){
    chilog(WARNING, "no packets actually timed out");
    return;
  }

  //chilog(WARNING, "lowest sequence # of all timed out packets was %d", lowestseq);
  //second pass
  //resend every packet with a seq equal to or greater than the one that timed out (go-back-n);
  if(0 == list_iterator_start(retrans_q)){
    chilog(DEBUG, "Retransq could not be iterated - second pass.");
    return;
  }
  i=0;
  while(list_iterator_hasnext(retrans_q)){
    cur = list_iterator_next(retrans_q);
    curpacket = cur->packet;
    //remove from queue and retransmit
    if(SEG_SEQ(curpacket) <= lowestseq){
      chilog(WARNING, "this packet timed out so we're retransmitting it");
      to_delete[i] = 1;
      list_append(&to_req, curpacket);
      //chilog(WARNING, "freeing variables associated with old reqmember");
      //don't free the packet yet, so don't call reqmember free
      pthread_mutex_destroy(&cur->lock4ack);
      pthread_cond_destroy(&cur->acked);
      free(cur);
    }
    else {
      chilog(WARNING, "this packet didn't time out so we're not resending it this time");
    }
    i++;
  }
  list_iterator_stop(retrans_q);

  //now delete the packets we are retransmitting from the queue
  for(j=0; j<lsize; j++){
    if(to_delete[j] == 1){
      deleted = list_delete_at(retrans_q, (j-ndel));
      ndel++;
      chilog(WARNING, "retrans_q length is %d, delete status was %d, j was %d, j-ndel was %d", list_size(retrans_q), deleted, j, j-ndel);
    }
  }
  free(to_delete);

  //chilog(WARNING,"unlocking retransq lock");
  pthread_mutex_unlock(lock_retrans_q);

  //now add all timed out packets to the queue
  list_iterator_start(&to_req);
  while(list_iterator_hasnext(&to_req)){
    curpacket = list_iterator_next(&to_req);
    chilog(WARNING, "sending packet from timeout process fun");
    chitcpd_send_tcp_packet(si, entry, curpacket);
    chitcpd_tcp_add_packet_to_retrans_q(curpacket, si, entry);
  }
  list_iterator_stop(&to_req);
  list_destroy(&to_req);

  process_q_after_ack(si, entry);
  //chilog(WARNING, "exiting timeout process function");
  return;
}


//when we get an ACK, goes through the queue and signals the CV of ACKed packets, killing their threads & freeing their structs
void process_q_after_ack(serverinfo_t * si, chisocketentry_t * entry){
  //chilog(WARNING, "entering process q after ack function");
  pthread_mutex_t *lock_retrans_q;
  list_t *retrans_q;
  reqmember_t *cur;
  tcp_packet_t *curpacket;
  tcp_data_t * tcp_data;
  int i = 0;
  int j = 0;
  int ndel = 0;

  tcp_data = &entry->socket_state.active.tcp_data;
  int unack = tcp_data->SND_UNA;
  retrans_q = &tcp_data->retrans_q;
  lock_retrans_q = &tcp_data->lock_retrans_q;
  int lsize = list_size(retrans_q);
  int * to_delete = calloc(sizeof(int)*lsize, 0);
  pthread_mutex_lock(lock_retrans_q);

  //iterate through queue, signal-ing to kill threads if their packets were acked, and delete acked packets' structs
  if(0 == list_iterator_start(retrans_q)){
    chilog(DEBUG, "Retransq could not be iterated after ack.");
    return;
  }

  //chilog(WARNING, "starting list iteration");
  while(list_iterator_hasnext(retrans_q)){
    cur = list_iterator_next(retrans_q);
    curpacket = cur->packet;
    //chilog(WARNING, "RETRANSQ PACKET (NULL=%i)", curpacket == NULL);
    //chilog(WARNING, "packet pointer is %d", curpacket);
    //chilog(WARNING, "our current send unacknowledged is %d", unack);
    chilog(WARNING, "below is the current packet we're looking at while processing an ack");
    chilog_tcp(WARNING, curpacket, LOG_OUTBOUND);

    //remove from queue and retransmit //seg_seq + len b/c what if we get an ack of half a packet b/c rcv buffer can only take so much
    //chilog(WARNING, "the last byte of the cur pack is %d", SEG_SEQ(curpacket)+SEG_LEN(curpacket));
    if(SEG_SEQ(curpacket)+SEG_LEN(curpacket) <= unack){ //i.e. all bytes in this packet have been acknowledged
      chilog(WARNING, "all bytes in the current packed have been acked, so signaling its thread and removing from q");
      
      //update rto
      struct timespec rtt = timespec_sub(tcp_data->mostrecentackin, cur->sent);
      chilog(WARNING, "rtt for this packet was sec = %d, nsec = %d", rtt.tv_sec, rtt.tv_nsec);
      update_rto(si, entry, rtt);

      //chilog(WARNING, "deleting from queue");
      to_delete[i] = 1;
      //int deleted = list_delete_at(retrans_q, i);
      //chilog(WARNING, "retrans_q length is %d, delete status was %d, i was %d", list_size(retrans_q), deleted, i);
      //chilog(WARNING, "freeing reqmember");
      //free_reqmember(cur); //includes freeing the packet
      pthread_cond_signal(&cur->acked);
    }
    i++;
  }
  list_iterator_stop(retrans_q);
  //now delete from list
  for(j=0; j<lsize; j++){
    if(to_delete[j] == 1){
      int deleted = list_delete_at(retrans_q, (j-ndel));
      ndel++;
      //chilog(WARNING, "retrans_q length is %d, delete status was %d, j was %d, j-ndel was %d", list_size(retrans_q), deleted, j, j-ndel);
    }
  }
  pthread_mutex_unlock(lock_retrans_q);
  //chilog(WARNING, "exiting ack processing function");
  return;
}

/* Out-of-order stuff */
/*
 * compare packets in a list by sequence #
 * 
 * this function compares two packets:
 * <0: p_one's seq # greater than p_two's
 * 0: sew # the same
 * >0: p_two's seq # greater than p_one's
 */
int packet_comparator(const void * p_one, const void * p_two) {
    /*compare sequence number */
    const tcp_packet_t * packet_one = (tcp_packet_t *)p_one;
    const tcp_packet_t * packet_two = (tcp_packet_t *)p_two;
    //chilog(WARNING, "packet one null status is %d, and two's null status is %d", packet_one==NULL, packet_two==NULL);
    return ( (SEG_SEQ(packet_one) < SEG_SEQ(packet_two)) - (SEG_SEQ(packet_one) > SEG_SEQ(packet_two)) );
}

/* return "match" when the sequence number matches the packet */
int packet_seeker(const void * packetEl, const void * seqKey) {
    const tcp_packet_t * packet = (tcp_packet_t *)packetEl;
    //chilog(WARNING, "packet null status is %d", packet==NULL);
    const uint32_t sequence = *((int *)seqKey);
    //chilog(WARNING, "sequence value is %zu", sequence);
    if(SEG_SEQ(packet) == sequence) {
        return 1;
    }
    return 0;
}

//adds a packet that comes after a gap to our out-of-order list & makes sure list is sorted
int chitcpd_tcp_add_p_to_oop(tcp_packet_t * out_of_order_packet, serverinfo_t * si, chisocketentry_t * entry) {
    tcp_data_t * tcp_data = &entry->socket_state.active.tcp_data;
    pthread_mutex_t * lock_oop_packet_list = &tcp_data->lock_oop_packet_list;
    list_t * oop_packet_list = &tcp_data->oop_packet_list;

    
    pthread_mutex_lock(lock_oop_packet_list);

    chilog(WARNING, "before adding this packet to the out-of-order list, len was %d", list_size(oop_packet_list));
    chilog_tcp(WARNING, out_of_order_packet, LOG_INBOUND); 
    list_prepend(oop_packet_list, out_of_order_packet);
    list_sort(oop_packet_list, 1); //sorts the list by ascending sequence number

    pthread_mutex_unlock(lock_oop_packet_list);
    chilog(WARNING, "after adding this packet to the out-of-order list, len was %d", list_size(oop_packet_list));    
    return 1;
}

//checks to see if we have the next packet (after the one we just got) in our oop list, and if so, triggers a packet_arrival on that packet
void process_oop_after_new_packet(serverinfo_t * si, chisocketentry_t * entry){ 
    tcp_data_t * tcp_data = &entry->socket_state.active.tcp_data;
    pthread_mutex_t * lock_oop_packet_list = &tcp_data->lock_oop_packet_list;
    list_t * oop_packet_list = &tcp_data->oop_packet_list;
    
    pthread_mutex_lock(lock_oop_packet_list);
    chilog(WARNING, "before trying to take things from OOP list if possible, len was %d", list_size(oop_packet_list));
    
    uint32_t * next_expected = &tcp_data->RCV_NXT; //the sequence # we're checking for in the oop_list -> note if multiple packets in list, just add one at a time and the fact that we get a packet_arrival event means that we call this function for the next one too
    //chilog(WARNING,"list point null status is %d and next_expected is %zu", oop_packet_list==NULL, *next_expected);
    tcp_packet_t * ret = list_seek(oop_packet_list, next_expected);
    if(ret!=NULL) { //if we've filled a gap
        chilog(WARNING, "we have the next packet we want in our oop list, so giving it, which is this to packet_arrival");
        tcp_packet_t * desired_packet = list_fetch(oop_packet_list); //should work because list sorted by ascending seq # at all times
        chilog_tcp(WARNING, desired_packet, LOG_INBOUND);
        pthread_mutex_lock(&tcp_data->lock_pending_packets);
        list_prepend(&tcp_data->pending_packets, desired_packet);
        pthread_mutex_unlock(&tcp_data->lock_pending_packets);
    }
    else {
        chilog(WARNING, "we didn't have the next packet in our OOP list");
    }
    chilog(WARNING, "after trying to take things from OOP list if possible, len was %d", list_size(oop_packet_list));
    pthread_mutex_unlock(lock_oop_packet_list);
}
    
/* End project 2b stuff */


//Given a payload, size of the payload, our current seq # and if we want it to be a fin or syn, creates & sends a packet
//additionally, starting in 2b, we add all fin, syn, and payload-containing packets to the retransmission queue
int chitcpd_tcp_send_packet(serverinfo_t * si, chisocketentry_t * entry, const uint8_t * buf, size_t len, bool_t fin, bool_t noack, int seq, bool_t syn) { //creats the packet, initializes the header, sends the packet, updates the variables
  tcp_data_t * our_data = &entry->socket_state.active.tcp_data;
  //chilog(WARNING, "Mallocing space for a packet");
  tcp_packet_t * packet_to_send = malloc(sizeof(tcp_packet_t));
  tcphdr_t * packet_header;
  //chilog(WARNING, "Making packet");
  chitcpd_tcp_packet_create(entry, packet_to_send, buf, len);
  packet_header = TCP_PACKET_HEADER(packet_to_send);
  //chilog(WARNING, "Setting header fields");
  packet_header->seq = chitcp_htonl(seq);
  packet_header->ack_seq = chitcp_htonl(our_data->RCV_NXT);
  packet_header->fin = fin;
  packet_header->syn = syn;
  packet_header->ack = !(noack); //fixed SYN ack problem
  packet_header->win = chitcp_htons(our_data->RCV_WND);
  //chilog(WARNING, "sending packet from our send packet fun");
  chilog_tcp(WARNING, packet_to_send, LOG_OUTBOUND);
  chitcpd_send_tcp_packet(si, entry, packet_to_send);
  if(fin||syn||len) { //if it's a fin, syn, or has a payload - should be everything other than an ack
    chitcpd_tcp_add_packet_to_retrans_q(packet_to_send, si, entry);
  }
  our_data->SND_NXT = our_data->SND_NXT + len;
  //free(buf);
  //free(packet_header);
  //free(packet_to_send);
  return 1;
}


/* When a PACKET_ARRIVAL event happens (i.e., when the peer sends us data/we have a pending packet), we call this function, which extracts a packet from the pending packet queue, extracts the data from those packets if there is a payload, verifies that the sequence numbers are correct and, if appropriate, puts the data in the receive buffer. Also deals with getting fins and acks and all that */
int chitcpd_tcp_handle_packet(serverinfo_t * si, chisocketentry_t *entry, bool_t resend) {
   
    tcp_data_t * our_data = &entry->socket_state.active.tcp_data;
    
    //get the packet
    pthread_mutex_lock(&our_data->lock_pending_packets);
    tcp_packet_t * incoming_packet = list_fetch(&our_data->pending_packets);
    tcphdr_t* incoming_header = TCP_PACKET_HEADER(incoming_packet);
    pthread_mutex_unlock(&our_data->lock_pending_packets);
    chilog(WARNING, "Getting packet...");  
    //chilog(WARNING, "incoming packet null status is %d", incoming_packet==NULL);
    chilog_tcp(WARNING, incoming_packet, LOG_INBOUND);
    
    //get packet data;
    tcp_state_t our_state = entry->tcp_state;
    int seq = SEG_SEQ(incoming_packet);
    int seg_ack = SEG_ACK(incoming_packet);
    int syn = incoming_header->syn;
    int ack = incoming_header->ack;
    int fin = incoming_header->fin;
    //chilog(WARNING, "Send Window prior to setting is %d", our_data->SND_WND); 
    our_data->SND_WND = SEG_WND(incoming_packet);
    circular_buffer_t * recv = &(our_data->recv);

    //stamp current time for calculating rto
    if(ack)
      clock_gettime(CLOCK_REALTIME, &our_data->mostrecentackin);
    
/* Cases for specific states */
/*if in CLOSED */
    if(our_state == CLOSED) {
        //chitcp_tcp_packet_free(incoming_packet);
    }

/* if in LISTEN */
    else if(our_state==LISTEN) {
        //chilog(WARNING, "In packet handler fun - Listen");
        //chilog(WARNING, "syn is %d", syn);
        if(syn) {
            //chilog(WARNING, "got a syn");
            //chilog(WARNING, "incoming packet has seg value %d", seq);
            our_data->RCV_NXT = seq +1;
            //chilog(WARNING, "updated receive next to be %d", our_data->RCV_NXT);
            our_data->IRS = seq;
	        circular_buffer_set_seq_initial(recv, our_data->IRS);
	        our_data->RCV_WND = circular_buffer_available(recv);
            chilog(WARNING, "Sending ACK/SYN...");  
            chitcpd_tcp_send_packet(si, entry, NULL, 0, 0, 0, our_data->ISS, 1);
            //chilog(WARNING, "Entering SYN_RCVD because we got a syn");
            chitcpd_update_tcp_state(si, entry, SYN_RCVD);
            //chitcp_tcp_packet_free(incoming_packet);
            return 1;
        }
    }

/*if in SYN-SENT */
    else if(our_state == SYN_SENT) {
                //chilog(WARNING, "In packet handler fun - Sent");
        if(ack) {
                chilog(WARNING, "Got an ack");
                
                //chilog(WARNING, "above test");
                //chilog(WARNING, "seg_ack is %d, SND_UNA is %d, SND_NXT is %d", seg_ack, our_data->SND_UNA, our_data->SND_NXT);
            if(our_data->SND_UNA <= seg_ack && seg_ack <= our_data->SND_NXT) {
                //chilog(WARNING, "below test");
                if(syn) {
                    //chilog(WARNING, "Got a syn");
		  our_data->SND_UNA = seg_ack;
		  chilog(WARNING, "processing rt q b/c we got a packet (with an ack) in");
		  process_q_after_ack(si, entry);
		  our_data->RCV_NXT = seq +1;
		  our_data->IRS = seq;
		  circular_buffer_set_seq_initial(recv, our_data->IRS);
		  our_data->RCV_WND = circular_buffer_available(recv);
		  if(our_data->SND_UNA > our_data->ISS) {
		    //chilog(WARNING, "Going into established");
		    chitcpd_update_tcp_state(si, entry, ESTABLISHED);
		    chitcpd_tcp_send_packet(si, entry, NULL, 0, 0, 0, our_data->SND_NXT, 0);
		    //chitcp_tcp_packet_free(incoming_packet);
		    return 1;
		  }
		  else {
		    //chilog(WARNING, "Going into received");
		    chitcpd_update_tcp_state(si, entry, SYN_RCVD);
		    chitcpd_tcp_send_packet(si, entry, NULL, 0, 0, 0, our_data->ISS, 1);
		    //chitcp_tcp_packet_free(incoming_packet);
		    return 1;
		  }
                }
            }
        }
    }




/*General case for all other states */
/*"Otherwise" has all the checks we need to do on the sequence numbers and what we do and don't send and all of that */
    else {
        //chilog(WARNING, "the current value of our receive next is %d", our_data->RCV_NXT);
        //chilog(WARNING, "got a packet while we're in state %d", our_state);
        if(our_data->RCV_WND == 0 && SEG_LEN(incoming_packet) > 0) {
            chilog(WARNING, "Unacceptable packet - as in WTF are you sending us!?!");
            chitcpd_tcp_send_packet(si, entry, NULL, 0, 0, 0, our_data->SND_NXT, 0);
            //chitcp_tcp_packet_free(incoming_packet);
            return 1;
        }

        if ( (seq == our_data->RCV_NXT) ) {
            chilog(WARNING, "Acceptable packet - as in the next one we're expecting");
            if(ack) {
	      chilog(WARNING, "We get an ack");
	      if(our_state == SYN_RCVD) {
		//chilog(WARNING, "We're in SYN_RCVD");
		if( (our_data->SND_UNA <= seg_ack) && (seg_ack <= our_data->SND_NXT) ) {
		  our_data->SND_UNA = seg_ack; 
		  chilog(WARNING, "processing rt q b/c we got a packet (with an ack) in");
		  process_q_after_ack(si, entry);
		  //chilog(WARNING, "entering into established from syn_rcvd b/c got an ack of our syn");
		  chitcpd_update_tcp_state(si, entry, ESTABLISHED);
          //got rid of acking the ack
		}
	      }
	      else if ((our_state == ESTABLISHED)||(our_state ==FIN_WAIT_1)||(our_state ==FIN_WAIT_2)||(our_state ==CLOSE_WAIT)||(our_state ==CLOSING)) {
		//chilog(WARNING, "In established or after");
		if( (our_data->SND_UNA < seg_ack ) && (seg_ack <= our_data->SND_NXT) ) {
		  //chilog(WARNING, "Updating SND_UNA");
		  //uint8_t throwaway[TCP_MSS + 100];
		  //circular_buffer_read(&our_data->send, throwaway, seg_ack - our_data->SND_UNA, 0);
		  our_data->SND_UNA = seg_ack;
		  chilog(WARNING, "processing rt q b/c we got a packet (with an ack) in");
		  process_q_after_ack(si, entry);
		}
		else if(seg_ack < our_data->SND_UNA) {
		  //chilog(WARNING, "we're doing nothing");
		}
		else if(seg_ack > our_data->SND_NXT) {
		  //chilog(WARNING, "ack > snd_NXT");
		  chitcpd_tcp_send_packet(si, entry, NULL, 0, 0, 0, our_data->SND_NXT, 0);
		  //chitcp_tcp_packet_free(incoming_packet);
		  return 1;
		}

		if(our_state == FIN_WAIT_1) { //check if our fin has been ACK'ed by seeing if SND_NXT = SND_UNA
		  //chilog(WARNING, "In handle packet fun - FIN_WAIT_1");
		  if(our_data->SND_UNA == our_data->SND_NXT) {
		    //chilog(WARNING, "about to enter FIN_WAIT_2 from fin wait 1 b/c got our fin acked");
		    chitcpd_update_tcp_state(si, entry, FIN_WAIT_2);
		  }
		}
                    
		else if(our_state == FIN_WAIT_2) {
		  //chilog(WARNING, "In handle packet fun - FIN_WAIT_2");
		  //close can be acknowledged if retransmission queue is empty -> will need to implement for part 2b
		}
                
		else if(our_state == CLOSING) {
		  //chilog(WARNING, "In handle packet fun - closing");
		  if(our_data->SND_UNA == our_data->SND_NXT) {
		    //chilog(WARNING, "about to enter TIME_WAIT then immediately CLOSED from closing b/c got an ack");
		    chitcpd_update_tcp_state(si, entry, TIME_WAIT);
		    chitcpd_update_tcp_state(si, entry, CLOSED);
		  }
		}
	      }
	      else if(our_state == LAST_ACK) {
		//chilog(WARNING, "In handle packet fun - last ack");
		//only thing we can be getting in last ack is ack of our fin so no test necessary
		//chilog(WARNING, "about to enter CLOSED from last ack b/c got an ack");
		chitcpd_update_tcp_state(si, entry, CLOSED);
		//chitcp_tcp_packet_free(incoming_packet);
		return 1;
	      }

	      if( (our_state==ESTABLISHED)||(our_state==FIN_WAIT_1)||(our_state==FIN_WAIT_2)) {
		//chilog(WARNING, "Processing segment text");
		//deliver segment text into the receive buffer - either until buffer is full or segment is empty
		int tocopy;
		if( TCP_PAYLOAD_LEN(incoming_packet) <= circular_buffer_available(recv) ) {
		  tocopy = TCP_PAYLOAD_LEN(incoming_packet);
		}
		else {
		  tocopy = circular_buffer_available(recv);
		}
		if(TCP_PAYLOAD_LEN(incoming_packet)>0) {
		  //chilog(WARNING, "sent us a packet with data");
		  uint8_t * packet_data = TCP_PAYLOAD_START(incoming_packet);
		  circular_buffer_write(recv, packet_data, tocopy, 0);
		  our_data->RCV_WND = circular_buffer_available(recv);
		  our_data->RCV_NXT = our_data->RCV_NXT + tocopy;
		  chilog(WARNING, "Sending a packet");
		  chitcpd_tcp_send_packet(si, entry, NULL, 0, 0, 0, our_data->SND_NXT, 0);
		}
		else {
		  //chilog(WARNING, "Sent us a packet with no data, i.e. just an ack");
		}
	      }

	      if(fin) {
		//chilog(WARNING, "Got a fin");
		if((our_state==SYN_RCVD)||(our_state==ESTABLISHED)) {
		  //chilog(WARNING, "Entering into CLOSE_WAIT from syn rcvd or established b/c got a fin");
		  chitcpd_tcp_send_packet(si, entry, NULL, 0, 0, 0, our_data->SND_NXT, 0);
		  chitcpd_update_tcp_state(si, entry, CLOSE_WAIT);                    
		}
		if(our_state==FIN_WAIT_1) {
		  //chilog(WARNING, "Getting a fin packet In fin wait 1 part of packet handler");
		  chitcpd_tcp_send_packet(si, entry, NULL, 0, 0, 0, our_data->SND_NXT, 0);
		  if(our_data->SND_UNA == our_data->SND_NXT) {
		    //chilog(WARNING, "Entering into time wait then immediately closed from fin wait 1 b/c got an ack");
		    chitcpd_update_tcp_state(si, entry, TIME_WAIT);
		    chitcpd_update_tcp_state(si, entry, CLOSED);
		  }
		  else {
		    //chilog(WARNING, "entering into closing from fin wait 1");
		    chitcpd_update_tcp_state(si, entry, CLOSING);
		  }                    
		}
		    
		if(our_state==FIN_WAIT_2) {
		  //chilog(WARNING, "in packet arrival function - in fin wait 2");
		  chitcpd_tcp_send_packet(si, entry, NULL, 0, 0, 0, our_data->SND_NXT, 0);
		  //chilog(WARNING, "entering into time wait then closed from fin wait 2 b/c got a packet");
		  chitcpd_update_tcp_state(si, entry, TIME_WAIT);
		  chitcpd_update_tcp_state(si, entry, CLOSED);
		}
		//chilog(WARNING, "about to close the circular buffer"); //bc we know no more incoming data if got a fin
		circular_buffer_close(&our_data->recv);    
		//chilog(WARNING, "just closed the circular buffer");
	      }
            }
        //chitcp_tcp_packet_free(incoming_packet);
        process_oop_after_new_packet(si, entry); //see if we can take anything out of oop & if so, do it!
        }

        else if ( ( (our_data->RCV_NXT <= seq) && (seq < our_data->RCV_NXT + our_data->RCV_WND) ) || ( (our_data->RCV_NXT <= seq+SEG_LEN(incoming_packet) - 1 ) && (seq+SEG_LEN(incoming_packet) - 1 < our_data->RCV_NXT + our_data->RCV_WND) ) ) {
            chilog(WARNING, "acceptable packet but out of order");
            //chilog(WARNING, "checking if we add null packet to oop, null status is %d", incoming_packet==NULL);
            chitcpd_tcp_add_p_to_oop(incoming_packet, si, entry);
        }
    }
    //chilog(WARNING, "Done with this packet and exiting packet handler fun");
    return 1;
}



/*Do we want a resend bit for project 2b? */
int chitcpd_tcp_process_send_buffer(serverinfo_t * si, chisocketentry_t * entry, bool_t resend) { //send stuff from the send buffer if you can
  tcp_data_t *tcp_data = &entry->socket_state.active.tcp_data;
  circular_buffer_t *send_buf = &(tcp_data->send);
  uint8_t *buf;
  uint8_t *packetbuf;
  int read;
  int x=0;
  int len;
  int y;
  int bufsize = tcp_data->SND_WND;
  //get all data to send into a linear buffer
  buf = malloc(sizeof(uint8_t)*(bufsize));
  //chilog(WARNING, "made a buf buffer of size %d", sizeof(uint8_t)*bufsize);
  read = circular_buffer_read(send_buf, buf, bufsize, 0);
  //chilog(WARNING, "Value of read is %d", read);
  //chilog(WARNING, "Value of send window is %d", tcp_data->SND_WND);
  //chilog(WARNING, "Value of send next is %d", tcp_data->SND_NXT);
  //segmentize as packets with maximum length TCP_MSS
  //we want to send x bytes, where x is the bytes we read
  //we read TCP_MSS bytes at a time from the linear buffer and send it out
  int size = tcp_data->SND_WND - (tcp_data->SND_NXT - tcp_data->SND_UNA); //compute effective window size
  while(x<read){
    //get availible size of the next packet: smallest of #bytes left, TCP_MSS, and send window
    if(read-x < TCP_MSS)
      len = read-x;
    else
      len = TCP_MSS;
    if(len > size)
      len = size;
    //copy from the linear buffer into the packet
    packetbuf = malloc(sizeof(uint8_t)*len);
    //chilog(WARNING, "made a packetbuf buffer of size %d", sizeof(uint8_t)*len);
    for(y=0; y<len; y++){
      packetbuf[y] = buf[y+x];
      //chilog(WARNING, "%d: %d from %d (len=%d)", y, packetbuf[y], y+x, len);
    }
    //x more bytes have been sent
    x += len;
    //send the packet
    //chilog(WARNING, "Sending packets that were broken up into MSS");
    //chilog(WARNING, "len is %d, read is %d, message is %s", len, read, packetbuf);
    chitcpd_tcp_send_packet(si, entry, packetbuf, len, 0, 0, tcp_data->SND_NXT, 0);
    //chilog(WARNING, "sent a packet, x is now %d and read was %d", x, read);
  } //loop
  free(buf);
  return 1;
}

int possible_chitcpd_tcp_process_send_buffer(serverinfo_t * si, chisocketentry_t * entry, bool_t resend) { 
  tcp_data_t *tcp_data = &entry->socket_state.active.tcp_data;
  circular_buffer_t *send_buf = &(tcp_data->send);
  uint8_t *buf, *packetbuff;
  int bufsize = tcp_data->SND_WND;
  int left = bufsize;
  int read,x,toread; 

  buf = calloc(sizeof(uint8_t)*TCP_MSS + 1, '\0');

  //while there is data to send, try to send as much as the reciever window is
  while(circular_buffer_count(send_buf) != 0 && left != 0){
    //read the min of bytes the reciever has left and tcpmss
    if(left < TCP_MSS)
      toread = left;
    else
      toread = TCP_MSS;

    read = circular_buffer_read(send_buf, buf, toread, 0);
    packetbuff = malloc(sizeof(uint8_t)*read);
    
    for(x=0; x<read; x++){
      packetbuff[x] = buf[x];
    }

    chitcpd_tcp_send_packet(si, entry, packetbuff, read, 0, 0, tcp_data->SND_NXT, 0);

    left -= read;
  }
  
  free(buf);
  return 1;
}

void tcp_data_init(tcp_data_t *tcp_data)
{
  list_init(&tcp_data->pending_packets);
  pthread_mutex_init(&tcp_data->lock_pending_packets, NULL);
  pthread_cond_init(&tcp_data->cv_pending_packets, NULL);
  list_init(&tcp_data->withheld_packets);
  pthread_mutex_init(&tcp_data->lock_withheld_packets, NULL);
  
  /* Initialization of additional tcp_data_t fields goes here */
  tcp_data->ISS = (rand() % 255) * 1000;
  tcp_data->SND_UNA = tcp_data->ISS;
  tcp_data->SND_NXT = tcp_data->ISS + 1;
  tcp_data->SND_WND = 1;
  circular_buffer_set_seq_initial(&tcp_data->send, tcp_data->ISS);
  tcp_data->finsent=0;
  
  /* 2b things */
  list_init(&tcp_data->retrans_q);
  pthread_mutex_init(&tcp_data->lock_retrans_q, NULL);
  list_init(&tcp_data->oop_packet_list);
  pthread_mutex_init(&tcp_data->lock_oop_packet_list, NULL);
  list_attributes_comparator(&tcp_data->oop_packet_list, packet_comparator);
  list_attributes_seeker(&tcp_data->oop_packet_list, packet_seeker);
  //list_sort(&tcp_data->oop_packet_list, 1); /* sorts packets by increasing segment number */
 
  //time data
  tcp_data->srtt = (struct timespec){.tv_sec = 0, .tv_nsec=5000000};
  tcp_data->rto = (struct timespec){.tv_sec = 0, .tv_nsec=5000000};
  tcp_data->alpha = 0.9;
  tcp_data->beta = 2;
  tcp_data->upper = (struct timespec) {.tv_sec = 0, .tv_nsec=999999999};
  tcp_data->lower = (struct timespec) {.tv_sec = 0, .tv_nsec=100000}; //reasonable value

  //will be updated with real values from syn packet
  tcp_data->IRS = 0;
  tcp_data->RCV_NXT = 0;
  tcp_data->RCV_WND = circular_buffer_available(&tcp_data->recv);
}


void tcp_data_free(tcp_data_t *tcp_data)
{
  circular_buffer_free(&tcp_data->send);
  circular_buffer_free(&tcp_data->recv);
  list_destroy(&tcp_data->pending_packets);
  pthread_mutex_destroy(&tcp_data->lock_pending_packets);
  pthread_cond_destroy(&tcp_data->cv_pending_packets);
  list_destroy(&tcp_data->withheld_packets);
  pthread_mutex_destroy(&(tcp_data->lock_withheld_packets));
  list_destroy(&tcp_data->retrans_q);
  pthread_mutex_destroy(&(tcp_data->lock_retrans_q));
  list_destroy(&tcp_data->oop_packet_list);
  pthread_mutex_destroy(&(tcp_data->lock_oop_packet_list));
  /* Cleanup of additional tcp_data_t fields goes here */
}


int chitcpd_tcp_state_handle_CLOSED(serverinfo_t *si, chisocketentry_t *entry, tcp_event_type_t event)
{
  if (event == APPLICATION_CONNECT)
    {
      //create TCB
      tcp_data_t *tcp_data = &entry->socket_state.active.tcp_data;
      tcp_data_init(tcp_data);
      //send syn packet
      //chilog(WARNING, "Sending packet from active opener");
      chitcpd_tcp_send_packet(si, entry, NULL, 0, 0, 1, tcp_data->ISS, 1);
      //update state
      chitcpd_update_tcp_state(si, entry, SYN_SENT);
    }
  else if (event == CLEANUP)
    {
      /* Any additional cleanup goes here */
    }
  else if (event == PACKET_ARRIVAL){
    chitcpd_tcp_handle_packet(si, entry, 0);
    
  }
  else
    chilog(WARNING, "In CLOSED state, received unexpected event.");
  return CHITCP_OK;
}


int chitcpd_tcp_state_handle_LISTEN(serverinfo_t *si, chisocketentry_t *entry, tcp_event_type_t event)
{
  if (event == PACKET_ARRIVAL)
    {
      chitcpd_tcp_handle_packet(si, entry, 0);
    }
  else
    chilog(WARNING, "In LISTEN state, received unexpected event.");
  return CHITCP_OK;
}


int chitcpd_tcp_state_handle_SYN_RCVD(serverinfo_t *si, chisocketentry_t *entry, tcp_event_type_t event)
{
  if (event == PACKET_ARRIVAL)
    {
      chitcpd_tcp_handle_packet(si, entry, 0);
    }
  else if (event == TIMEOUT)
    {
      process_q_after_ack(si, entry);
      process_timeout(si, entry);
    }
  else
    chilog(WARNING, "In SYN_RCVD state, received unexpected event.");
  return CHITCP_OK;
}


int chitcpd_tcp_state_handle_SYN_SENT(serverinfo_t *si, chisocketentry_t *entry, tcp_event_type_t event)
{
  if (event == PACKET_ARRIVAL)
    {
      chitcpd_tcp_handle_packet(si, entry, 0);
    }
  else if (event == TIMEOUT)
    {
      process_q_after_ack(si, entry);
      process_timeout(si, entry);
    }
  else
    chilog(WARNING, "In SYN_SENT state, received unexpected event.");
  return CHITCP_OK;
}


int chitcpd_tcp_state_handle_ESTABLISHED(serverinfo_t *si, chisocketentry_t *entry, tcp_event_type_t event)
{
  tcp_data_t *tcp_data = &entry->socket_state.active.tcp_data;
  circular_buffer_t *send_buf = &(tcp_data->send);
  if (event == APPLICATION_SEND)
    {
      //chilog(WARNING, "entering App send");
      chitcpd_tcp_process_send_buffer(si, entry, 0);
    }
  else if (event == PACKET_ARRIVAL) //handle if we get a FIN packet in in our handle_packet function
    {
      //chilog(WARNING, "Got a packet in ESTABLISHED");
      chitcpd_tcp_handle_packet(si, entry, 0);
    }
  else if (event == APPLICATION_RECEIVE)
    {
      // ONLY THING WE DO HERE IS UPDATE THE TCP VARIABLES B/C SPACE IS NOW AVAILABLE IN THE BUFFER
      circular_buffer_t *recv_buf = &(tcp_data->recv);
      tcp_data->RCV_WND = circular_buffer_available(recv_buf);
    }
  /* Your code goes here */
  //remember we usually don't IMMEDIATELY send a FIN packet, we have to wait until all outstanding data has been sent from our send buffer
  //closes the buffer, makes sure that nothing else can be written to it, but DOES need to send our stuff off before we send the FIN
  else if (event == APPLICATION_CLOSE)
    {
      //chilog(WARNING, "Got an app close in established, so saying we got a close request and setting the bit and blocking the send buffer from getting anything else put into it");
      tcp_data->closing = 1;
      circular_buffer_close(send_buf);
      if(circular_buffer_count(send_buf) == 0) { //if the send buffer is empty, we're done and can send our fin
	//chilog(WARNING, "Send buffer was empty in ESTABLISHED when we got an application close so we see if we have to send a FIN");
	if( (tcp_data->finsent==0) ) {
	  //chilog(WARNING, "haven't sent a FIN, so sending one now in APP CLOSE");
	  //tcp_data->SND_NXT = tcp_data->SND_NXT+1;
	  chitcpd_tcp_send_packet(si, entry, NULL, 0, 1, 0, tcp_data->SND_NXT, 0);
	  tcp_data->finsent = 1;
	}
	else {
	  //chilog(WARNING, "Already sent a fin so don't send one now");
	}
      }
      else { //we try to empty our send buffer and transition to FIN_WAIT_1 where we'll check if its empty
	chitcpd_tcp_process_send_buffer(si, entry, 0);
	//chilog(WARNING, "send buffer was not empty in ESTABLISHED when we got an app close");
      }
      //chilog(WARNING, "going into FIN_WAIT_1 from ESTABLISHED b/c we got an app close regardless of whether or not send buffer was full");
      chitcpd_update_tcp_state(si, entry, FIN_WAIT_1); //don't queue this
    }
  else if (event == TIMEOUT)
    {
      process_q_after_ack(si, entry);
      process_timeout(si, entry);
    }
  else
    chilog(WARNING, "In ESTABLISHED state, received unexpected event (%i).", event);
  return CHITCP_OK;
}


int chitcpd_tcp_state_handle_FIN_WAIT_1(serverinfo_t *si, chisocketentry_t *entry, tcp_event_type_t event)
{
  //chilog(WARNING, "Running fin wait 1 handler");
  tcp_data_t *tcp_data = &entry->socket_state.active.tcp_data;
  circular_buffer_t *send_buf = &(tcp_data->send);
  if (event == PACKET_ARRIVAL)
    {
      chitcpd_tcp_handle_packet(si, entry, 0);
      if( (circular_buffer_count(send_buf) == 0) && (tcp_data->closing ==1)) { //when we've finally sent everything and gotten it all acked, then we send a FIN and go to closed
	// chilog(WARNING, "our send buffer became empty in fin wait 1 after we had gotten a close, so seeing if we have to send a fin & going to closing");
	if( (tcp_data->finsent==0) ) {
	  //chilog(WARNING, "haven't sent a FIN, so sending one now");
	  //tcp_data->SND_NXT = tcp_data->SND_NXT+1;
	  chitcpd_tcp_send_packet(si, entry, NULL, 0, 1, 0, tcp_data->SND_NXT, 0);
	  tcp_data->finsent = 1;
	}
	else {
	  //chilog(WARNING, "Already sent a fin so don't send one now");
	}
      }
      else {
	//chilog(WARNING, "send buffer not yet empty in fin wait 1"); //try to empty it
	chitcpd_tcp_process_send_buffer(si, entry, 0);
      }
    }
  else if (event == TIMEOUT)
    {
      process_q_after_ack(si, entry);
      process_timeout(si, entry);
    }
  else
    chilog(WARNING, "In FIN_WAIT_1 state, received unexpected event (%i).", event);
  return CHITCP_OK;
}


int chitcpd_tcp_state_handle_FIN_WAIT_2(serverinfo_t *si, chisocketentry_t *entry, tcp_event_type_t event)
{
  //chilog(WARNING, "In fin wait 2's handlers");
  if (event == PACKET_ARRIVAL)
    {
      chitcpd_tcp_handle_packet(si, entry, 0);
    }
  else if (event == TIMEOUT)
    {
      process_q_after_ack(si, entry);
      process_timeout(si, entry);
    }
  else
    chilog(WARNING, "In FIN_WAIT_2 state, received unexpected event (%i).", event);
  return CHITCP_OK;
}


int chitcpd_tcp_state_handle_CLOSE_WAIT(serverinfo_t *si, chisocketentry_t *entry, tcp_event_type_t event)
{
  //chilog(WARNING, "in close wait's handler");
  tcp_data_t *tcp_data = &entry->socket_state.active.tcp_data;
  circular_buffer_t * send_buf = &tcp_data->send;
  if (event == APPLICATION_CLOSE)
    {
      //chilog(WARNING, "got a close request in close wait so setting close bit & blocking buffer");
      tcp_data->closing = 1;
      circular_buffer_close(send_buf);
      if(circular_buffer_count(send_buf) == 0) { //if we're empty, send a FIN, and go to CLOSING
	//chilog(WARNING, "send buffer was empty in CLOSE_WAIT, so seeing if we have to send a FIN and going to LAST_ACK");
	if( (tcp_data->finsent==0) ) {
	  //chilog(WARNING, "haven't sent a FIN, so sending one now");
	  //tcp_data->SND_NXT = tcp_data->SND_NXT+1;
	  chitcpd_tcp_send_packet(si, entry, NULL, 0, 1, 0, tcp_data->SND_NXT, 0);
	  tcp_data->finsent = 1;
	}
	else {
	  //chilog(WARNING, "Already sent a fin so don't send one now");
	}
	chitcpd_update_tcp_state(si, entry, LAST_ACK);
      }
      else {
	chitcpd_tcp_process_send_buffer(si, entry, 0); //try to send it
	//chilog(WARNING, "staying in CLOSE_WAIT because send buffer not yet empty");
      }
    }
  else if (event == PACKET_ARRIVAL)
    {
      chitcpd_tcp_handle_packet(si, entry, 0);
      if( (circular_buffer_count(send_buf) == 0) && (tcp_data->closing ==1) ) { //when we're empty, send our fin and go to closing
	//chilog(WARNING, "send buffer became empty after got a close request in CLOSE WAIT so sending fin & going to closing");
	if( (tcp_data->finsent==0) ) {
	  //chilog(WARNING, "haven't sent a FIN, so sending one now");
	  //tcp_data->SND_NXT = tcp_data->SND_NXT+1;
	  chitcpd_tcp_send_packet(si, entry, NULL, 0, 1, 0, tcp_data->SND_NXT, 0);
	  tcp_data->finsent = 1;
	}
	else {
	  //chilog(WARNING, "Already sent a fin so don't send one now");
	}
	chitcpd_update_tcp_state(si, entry, CLOSING);
      }
      else {
	//chilog(WARNING, "send buffer not yet empty in close wait");
      }
    }
  else if (event == TIMEOUT)
    {
      process_q_after_ack(si, entry);
      process_timeout(si, entry);
    }
  else
    chilog(WARNING, "In CLOSE_WAIT state, received unexpected event (%i).", event);
  return CHITCP_OK;
}


int chitcpd_tcp_state_handle_CLOSING(serverinfo_t *si, chisocketentry_t *entry, tcp_event_type_t event)
{
  if (event == PACKET_ARRIVAL)
    {
      chitcpd_tcp_handle_packet(si, entry, 0);
    }
  else if (event == TIMEOUT)
    {
      process_q_after_ack(si, entry);
      process_timeout(si, entry);
    }
  else
    chilog(WARNING, "In CLOSING state, received unexpected event (%i).", event);
  return CHITCP_OK;
}


int chitcpd_tcp_state_handle_TIME_WAIT(serverinfo_t *si, chisocketentry_t *entry, tcp_event_type_t event)
{
  chilog(WARNING, "Running handler for TIME_WAIT. This should not happen.");
  return CHITCP_OK;
}


int chitcpd_tcp_state_handle_LAST_ACK(serverinfo_t *si, chisocketentry_t *entry, tcp_event_type_t event)
{
  if (event == PACKET_ARRIVAL)
    {
      chitcpd_tcp_handle_packet(si, entry, 0);
    }
  else
    chilog(WARNING, "In LAST_ACK state, received unexpected event (%i).", event);
  return CHITCP_OK;
}
