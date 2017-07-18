/*
 *  chiTCP - A simple, testable TCP stack
 *
 *  Implementation of the TCP protocol.
 *
 *  chiTCP follows a state machine approach to implementing TCP.
 *  This means that there is a handler function for each of
 *  the TCP states (CLOSED, LISTEN, SYN_RCVD, etc.). If an
 *  event (e.g., a packet arrives) while the connection is
 *  in a specific state (e.g., ESTABLISHED), then the handler
 *  function for that state is called, along with information
 *  about the event that just happened.
 *
 *  Each handler function has the following prototype:
 *
 *  int f(serverinfo_t *si, chisocketentry_t *entry, tcp_event_type_t event);
 *
 *  si is a pointer to the chiTCP server info. The functions in
 *       this file will not have to access the data in the server info,
 *       but this pointer is needed to call other functions.
 *
 *  entry is a pointer to the socket entry for the connection that
 *          is being handled. The socket entry contains the actual TCP
 *          data (variables, buffers, etc.), which can be extracted
 *          like this:
 *
 *            tcp_data_t *tcp_data = &entry->socket_state.active.tcp_data;
 *
 *          Other than that, no other fields in "entry" should be read
 *          or modified.
 *
 *  event is the event that has caused the TCP thread to wake up. The
 *          list of possible events corresponds roughly to the ones
 *          specified in http://tools.ietf.org/html/rfc793#section-3.9.
 *          They are:
 *
 *            APPLICATION_CONNECT: Application has called socket_connect()
 *            and a three-way handshake must be initiated.
 *
 *            APPLICATION_SEND: Application has called socket_send() and
 *            there is unsent data in the send buffer.
 *
 *            APPLICATION_RECEIVE: Application has called socket_recv() and
 *            any received-and-acked data in the recv buffer will be
 *            collected by the application (up to the maximum specified
 *            when calling socket_recv).
 *
 *            APPLICATION_CLOSE: Application has called socket_close() and
 *            a connection tear-down should be initiated.
 *
 *            PACKET_ARRIVAL: A packet has arrived through the network and
 *            needs to be processed (RFC 793 calls this "SEGMENT ARRIVES")
 *
 *            TIMEOUT: A timeout (e.g., a retransmission timeout) has
 *            happened.
 *
 */

/*
 *  Copyright (c) 2013-2014, The University of Chicago
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or withsend
 *  modification, are permitted provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  - Neither the name of The University of Chicago nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software withsend specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY send OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
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

//does stuff like initialize a packet with the header - also definitely want this function
/*THE ONLY WAY WE EVER WILL INTERFACE WITH THE NETWORK LAYER -> IN PROJECT 1, THIS WILL MAKE SURE THAT THE PACKET WE PUT INTO IT GETS SENT AWAY AND IN PROJECT 2 WE'LL HAVE TO DO STUFF LIKE ADDING A RETRANSMISSION TIMER */
int chitcpd_tcp_send_packet(serverinfo_t * si, chisocketentry_t * entry, const uint8_t * buf, size_t len, bool_t fin, bool_t resend, int seq, bool_t syn) { //creats the packet, initializes the header, sends the packet, updates the variables
        tcp_data_t * our_data = &entry->socket_state.active.tcp_data;        
        tcp_packet_t * packet_to_send = malloc(sizeof(tcp_packet_t));
        tcphdr_t * packet_header;
        chitcpd_tcp_packet_create(entry, packet_to_send, buf, len);
        packet_header = TCP_PACKET_HEADER(packet_to_send);
        packet_header->seq = htonl(seq);
        packet_header->ack_seq = htonl(our_data->RCV_NXT);
        packet_header->fin = fin;
        packet_header->syn = syn;
        packet_header->ack = 1;
	packet_header->win = our_data->RCV_WND;
        chilog_tcp(WARNING, packet_to_send, LOG_OUTBOUND);
        chitcpd_send_tcp_packet(si, entry, packet_to_send);
        our_data->SND_NXT = our_data->SND_NXT + len;
        //our_data->SND_WND = our_data->SND_WND - len;
        return 1;        
}



/*"Otherwise" has all the checks we need to do on the sequence numbers and what we do and don't send and all of that -> We'll be able to implement this very methodically and sanely, like OH MY GOD do this immediately
Borja's implementation is ~400 lines long, jsyk */
int chitcpd_tcp_handle_packet(serverinfo_t * si, chisocketentry_t *entry, bool_t resend) {
   
/* When a PACKET_ARRIVAL event happens (i.e., when the peer sends us data), you must extract the packets from the pending packet queue, extract the data from those packets, verify that the sequence numbers are correct and, if appropriate, put the data in the receive buffer. */

    tcp_data_t * our_data = &entry->socket_state.active.tcp_data;
    
    pthread_mutex_lock(&our_data->lock_pending_packets);
    tcp_packet_t * incoming_packet = list_fetch(&our_data->pending_packets); //remember to chitcp_tcp_packet_free(our_packet) when done
    //note we should want to fetch all pending packets, right? or is this function for just one and in the rest of our functions we loop through (while list_not_empty) do this function?
    tcphdr_t* incoming_header = TCP_PACKET_HEADER(incoming_packet);
    pthread_mutex_unlock(&our_data->lock_pending_packets);
    chilog(WARNING, "Getting packet...");  
    chilog_tcp(WARNING, incoming_packet, LOG_INBOUND);
    tcp_state_t our_state = entry->tcp_state;
    int seq = SEG_SEQ(incoming_packet);
    int seg_ack = SEG_ACK(incoming_packet);
    int syn = incoming_header->syn;
    int ack = incoming_header->ack;
    int fin = incoming_header->fin;
    chilog(WARNING, "Send Window prior to setting is %d", our_data->SND_WND); 
    our_data->SND_WND = SEG_WND(incoming_packet);
    //can we just do an update of RCV.NXT and send back an ACK here? - no, only when get a segment
    //when get an ACK, instead, we cwant to advance SND.UNA
    //when we create a segment and send it, we need to advance SND.NXT


    circular_buffer_t * recv = &(our_data->recv);

    

/* Cases for specific states */
/*if in CLOSED */
    if(our_state == CLOSED) {
        //do nothing
    }

/* if in LISTEN */
    else if(our_state==LISTEN) {
        chilog(WARNING, "In Listen");
        chilog(WARNING, "syn is %d", syn);
        if(syn) {
            chilog(WARNING, "got a syn");
            chilog(WARNING, "incoming packet has seg value %d", seq);
            our_data->RCV_NXT = seq +1;
            chilog(WARNING, "updated receive next to be %d", our_data->RCV_NXT);
            our_data->IRS = seq;
	    circular_buffer_set_seq_initial(recv, our_data->IRS);
	    our_data->RCV_WND = circular_buffer_available(recv);
            chilog(WARNING, "Sending ACK/SYN...");  
            chitcpd_tcp_send_packet(si, entry, NULL, 0, 0, 0, our_data->ISS, 1);
            chilog(WARNING, "Entering SYN_RCVD");
            chitcpd_update_tcp_state(si, entry, SYN_RCVD);
            chitcp_tcp_packet_free(incoming_packet);
            return 1;
        }
    }

/*if in SYN-SENT */
    else if(our_state == SYN_SENT) {
                chilog(WARNING, "In Sent");
        if(ack) {
                chilog(WARNING, "Got an ack");
                
                chilog(WARNING, "above test");
                chilog(WARNING, "seg_ack is %d, SND_UNA is %d, SND_NXT is %d", seg_ack, our_data->SND_UNA, our_data->SND_NXT);
            if(our_data->SND_UNA <= seg_ack && seg_ack <= our_data->SND_NXT) {
                chilog(WARNING, "below test");
                if(syn) {
                    chilog(WARNING, "Got a syn");
                    our_data->RCV_NXT = seq +1;
                    our_data->IRS = seq;
		    circular_buffer_set_seq_initial(recv, our_data->IRS);
		    our_data->RCV_WND = circular_buffer_available(recv);
                    our_data->SND_UNA = seg_ack;
                    if(our_data->SND_UNA > our_data->ISS) {
                        chilog(WARNING, "Going into established");
                        chitcpd_update_tcp_state(si, entry, ESTABLISHED);
                        chitcpd_tcp_send_packet(si, entry, NULL, 0, 0, 0, our_data->SND_NXT, 0);
                        chitcp_tcp_packet_free(incoming_packet);
                        return 1;
                    }
                    else {
                        chilog(WARNING, "Going into received");
                        chitcpd_update_tcp_state(si, entry, SYN_RCVD);
                        chitcpd_tcp_send_packet(si, entry, NULL, 0, 0, 0, our_data->ISS, 1);
                        chitcp_tcp_packet_free(incoming_packet);
                        return 1;
                    }
                }
            }
        }
    }




/*General case for all other states */
    else {
        if(our_data->RCV_WND == 0 && SEG_LEN(incoming_packet) > 0) { //unacceptable 
            chilog(WARNING, "Unacceptable packet");
            chitcpd_tcp_send_packet(si, entry, NULL, 0, 0, 0, our_data->SND_NXT, 0);
            chitcp_tcp_packet_free(incoming_packet);
            return 1;
        }
        if ( (seq == our_data->RCV_NXT) || ( (our_data->RCV_NXT <= seq) && (seq < our_data->RCV_NXT + our_data->RCV_WND) ) || ( (our_data->RCV_NXT <= seq+SEG_LEN(incoming_packet) - 1 ) && (seq+SEG_LEN(incoming_packet) - 1 < our_data->RCV_NXT + our_data->RCV_WND) ) ) { //acceptable
            chilog(WARNING, "Acceptable packet");
            if(ack) {
                chilog(WARNING, "We get an ack");
                if(our_state == SYN_RCVD) {
                    chilog(WARNING, "We're in SYN_RCVD");
                    if( (our_data->SND_UNA <= seg_ack) && (seg_ack <= our_data->SND_NXT) ) {
                       our_data->SND_UNA = seg_ack; 
                        chilog(WARNING, "Should update");
                        chitcpd_update_tcp_state(si, entry, ESTABLISHED);
                        chitcpd_tcp_send_packet(si, entry, NULL, 0, 0, 0, our_data->SND_NXT, 1);
                    }
                }
                else if (our_state == ESTABLISHED||FIN_WAIT_1||FIN_WAIT_2||CLOSE_WAIT||CLOSING) {
                    chilog(WARNING, "In established or after");
                    if( (our_data->SND_UNA < seg_ack ) && (seg_ack <= our_data->SND_NXT) ) {
                        chilog(WARNING, "Updating SND_UNA");
			uint8_t throwaway[TCP_MSS + 100];
			circular_buffer_read(&our_data->send, throwaway, seg_ack - our_data->SND_UNA, 0);
                        our_data->SND_UNA = seg_ack;
                    }
                    else if(seg_ack < our_data->SND_UNA) {
                    }
                    else if(seg_ack > our_data->SND_NXT) {
                        chilog(WARNING, "ack > snd_NXT");
                        chitcpd_tcp_send_packet(si, entry, NULL, 0, 0, 0, our_data->SND_NXT, 0);
                        chitcp_tcp_packet_free(incoming_packet);
                        return 1;
                    }
                    if(our_state == FIN_WAIT_1) { //how do we see if our fin has been ACK'ed? if SEG_ACK = SND_UNA??
                        chilog(WARNING, "In FIN_WAIT_1");
                        if(seg_ack == our_data->SND_UNA+1) {
                            chitcpd_update_tcp_state(si, entry, FIN_WAIT_2);
                        }
                    }
                    
                    else if(our_state == FIN_WAIT_2) {
                        chilog(WARNING, "In FIN_WAIT_2");
                       //close can be acknowledged if retransmission queue is empty -> will need to implement for part 2
                       //unclear from RFC if we should send back an ACK or not, guesssing we should right now
                        chitcpd_tcp_send_packet(si, entry, NULL, 0, 0, 0, our_data->SND_NXT, 0);
                    }
                
                    else if(our_state == CLOSING) {
                            chilog(WARNING, "In closing");
                            if(seg_ack == our_data->SND_UNA+1) {
                                chitcpd_update_tcp_state(si, entry, TIME_WAIT);
                        }
                    }
                }
                else if(our_state == LAST_ACK) {
                    chilog(WARNING, "In last ack");
                    if(seg_ack == our_data->SND_UNA+1) {
                            tcp_data_free(our_data); //NOTE WE ALREADY HAD THIS
                            chitcpd_update_tcp_state(si, entry, CLOSED);
                            chitcp_tcp_packet_free(incoming_packet);
                            return 1;
                    }
                }

                else if(our_state == TIME_WAIT) { //acknowledge it and re-timeout
                    chilog(WARNING, "In Time wait");
                    //could have, but automatically true if(seg_ack == our_data->SND_UNA+1) {
                    chitcpd_tcp_send_packet(si, entry, NULL, 0, 0, 0, our_data->SND_NXT, 0);
                    sleep(2*60);
                    chitcpd_timeout(si, entry);
                    //tcp_data_free(our_data)?
                    //}
                }

                if(our_state==ESTABLISHED||FIN_WAIT_1||FIN_WAIT_2) {
                    chilog(WARNING, "Processing segment text");
                    /*Process the segment text */
                    //deliver segment text into the receive buffer - either until buffer is full or segment is empty
                    //pthread_mutex_lock(&recv->lock);
                    int tocopy;
                    if( TCP_PAYLOAD_LEN(incoming_packet) <= circular_buffer_available(recv) ) {
                        tocopy = TCP_PAYLOAD_LEN(incoming_packet);
                    }
                    else {
                        tocopy = circular_buffer_available(recv);
                    }
                    if(TCP_PAYLOAD_LEN(incoming_packet)>0) {
                        chilog(WARNING, "sent us a packet with data");
                        uint8_t * packet_data = TCP_PAYLOAD_START(incoming_packet);
                        circular_buffer_write(recv, packet_data, tocopy, 0);
                        our_data->RCV_WND = circular_buffer_available(recv);
                        our_data->RCV_NXT = our_data->RCV_NXT + tocopy;
                        chilog(WARNING, "Send a packet");
                        chitcpd_tcp_send_packet(si, entry, NULL, 0, 0, 0, our_data->SND_NXT, 0);
                    }
                    else {
                        chilog(WARNING, "Sent us a packet with no data, i.e. just an ack");
                    }
		    //pthread_mutex_unlock(&recv->lock);
                }

                if(fin) {
                    chilog(WARNING, "Got a fin");
                    if(our_state==SYN_RCVD||ESTABLISHED) {
                        chitcpd_update_tcp_state(si, entry, CLOSE_WAIT);                    
                    }
                    if(our_state==FIN_WAIT_1) {
                        if(seg_ack == our_data->SND_UNA+1) {
                            chitcpd_update_tcp_state(si, entry, TIME_WAIT);
                            sleep(2*60);
                            chitcpd_timeout(si, entry);
                        }
                        else {
                            chitcpd_update_tcp_state(si, entry, CLOSING);
                        }                    
                    }
                    if(our_state==FIN_WAIT_2) {
                        chitcpd_update_tcp_state(si, entry, TIME_WAIT);
                        sleep(2*60);
                        chitcpd_timeout(si, entry);
                    }
                    if(our_state==TIME_WAIT) {
                        sleep(2*60);
                        chitcpd_timeout(si, entry);
                        chitcp_tcp_packet_free(incoming_packet);
                        return 1;
                    }
                        
                }
            }
        
        }
    }
    chilog(WARNING, "Done with this packet");
    chitcp_tcp_packet_free(incoming_packet);
    return 1;
}

int chitcpd_tcp_process_send_buffer(serverinfo_t * si, chisocketentry_t * entry, bool_t resend) { //send stuff if you can
<<<<<<< HEAD
  int len, read;
  tcp_data_t *tcp_data = &entry->socket_state.active.tcp_data;
  uint8_t *msg;

  //figure out how much data we can send
  if(tcp_data->SND_WND < TCP_MSS){
    len = tcp_data->SND_WND;
  } else {
    len = TCP_MSS;
  }
  
  //get that data
  msg = calloc(sizeof(uint8_t)*len, '\0');
  read = circular_buffer_peek(&tcp_data->send, msg, len, 0);
  chilog(WARNING, "len is %d, read is %d, message is %s", len, read, msg);

  //send the message out
  chitcpd_tcp_send_packet(si, entry, msg, read, 0, 0, tcp_data->SND_NXT, 0);

  return 1;
=======
    chilog(WARNING, "entering send buffer function");
    tcp_data_t *tcp_data = &entry->socket_state.active.tcp_data;
	circular_buffer_t *send_buf = &(tcp_data->send);
	uint8_t *buf;
	uint8_t *packetbuf;
	int read;
    
	int x=0;
	int len;
	int y;

	//get all data to send
	buf = malloc(sizeof(uint8_t)*(tcp_data->SND_WND));
	//pthread_mutex_lock(&send_buf->lock);
	read = circular_buffer_peek(send_buf, buf, (tcp_data->SND_WND), 0);
	chilog(WARNING, "Value of read is %d", read);
	chilog(WARNING, "Value of send window is %d", tcp_data->SND_WND);
	//pthread_mutex_unlock(&send_buf->lock);
	//send all bytes as packets with maximum length TCP_MSS
	while(x<read){

	  //get availible size
	  if(read-x < TCP_MSS)
	    len = read-x;
	  else
	    len = TCP_MSS;

	  //break it up into MSS
	  packetbuf = calloc(sizeof(uint8_t)*len,'\0');
	  for(y=0; y<len; y++){
	    packetbuf[y] = buf[y+x];
        chilog(WARNING, "%d", packetbuf[y]);
	  }
	  x += len;
	  //send the packet
      chilog(WARNING, "Sending packets that were broken up into MSS");
	  chitcpd_tcp_send_packet(si, entry, packetbuf, read, 0, 0, tcp_data->SND_NXT, 0);
	
	}

	free(buf);
    

    return 1;
>>>>>>> 9caf708c105466d928df4c15cd7ddba7cc9e292e
}

void tcp_data_init(tcp_data_t *tcp_data)
{
    list_init(&tcp_data->pending_packets);
    pthread_mutex_init(&tcp_data->lock_pending_packets, NULL);
    pthread_cond_init(&tcp_data->cv_pending_packets, NULL);
    list_init(&tcp_data->withheld_packets);
    pthread_mutex_init(&tcp_data->lock_withheld_packets, NULL);
    
    //not sure we need these
    //circular_buffer_init(tcp_data->recv, 1000);
    //circular_buffer_init(tcp_data->send, 1000);

    /* Initialization of additional tcp_data_t fields goes here */
    tcp_data->ISS = (rand() % 255) * 1000;
    tcp_data->SND_UNA = tcp_data->ISS;
    tcp_data->SND_NXT = tcp_data->ISS + 1;
    tcp_data->SND_WND = 1; 
    circular_buffer_set_seq_initial(&tcp_data->send, tcp_data->ISS);
    
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

    /* Cleanup of additional tcp_data_t fields goes here */
}


//remember most will conclude or something with chitcpd_update_tcp_state(si, entry, NEWSTATE);

int chitcpd_tcp_state_handle_CLOSED(serverinfo_t *si, chisocketentry_t *entry, tcp_event_type_t event)
{

  if (event == APPLICATION_CONNECT)
    {
      //create TCB
      tcp_data_t *tcp_data = &entry->socket_state.active.tcp_data;
      tcp_data_init(tcp_data);
     
      //send syn packet
      chilog(WARNING, "Sending packet from active opener");
      chitcpd_tcp_send_packet(si, entry, NULL, 0, 0, 0, tcp_data->ISS, 1);

      //update state
      chitcpd_update_tcp_state(si, entry, SYN_SENT);
    }
    else if (event == CLEANUP)
    {
        /* Any additional cleanup goes here */
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
    /* Your code goes here */
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
    /* Your code goes here */
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
    chilog(WARNING, "entering App send");
	uint8_t *buf;
	uint8_t *packetbuf;
	int read;
    
	int x=0;
	int len;
	int y;
	/*
	//warning, if snd_wnd is greater than tcp_mss, it will send too much data

	//get all data to send
	buf = malloc(sizeof(uint8_t)*(100));
	read = circular_buffer_peek(send_buf, buf, 100, 0);//(tcp_data->SND_WND), 0);
	chilog(WARNING, "Value of read is %d", read);
	chilog(WARNING, "Value of send window is %d", tcp_data->SND_WND);
	//send all bytes as packets with maximum length TCP_MSS
	while(x<read){

	  //get availible size
	  if(read-x < tcp_data->SND_WND)
	    len = read-x;
	  else
	    len = tcp_data->SND_WND;
	  chilog(WARNING, "len is %d", len);
	  //break it up into MSS
	  packetbuf = calloc(sizeof(uint8_t)*len,'\0');
	  for(y=0; y<len; y++){
	    packetbuf[y] = buf[y+x];
	    chilog(WARNING, "%d", packetbuf[y]);
	  }
	  x += len;
	  //send the packet
	  chilog(WARNING, "Sending packets that were broken up into MSS");
	  chitcpd_tcp_send_packet(si, entry, packetbuf, len, 0, 0, tcp_data->SND_NXT, 0);
	
	}

	free(buf);
	*/
	
	while(tcp_data->SND_UNA + circular_buffer_count(send_buf) - tcp_data->SND_NXT > 0){
	  if(tcp_data->SND_NXT == tcp_data->SND_UNA) //do we need to send anything
	    chitcpd_tcp_process_send_buffer(si, entry, 0);
	}
    }
    else if (event == PACKET_ARRIVAL)
    {
         chitcpd_tcp_handle_packet(si, entry, 0);
        if( (circular_buffer_count(send_buf) == 0) && (tcp_data->closing ==1) ) {
             chitcpd_tcp_send_packet(si, entry, NULL, 0, 1, 0, tcp_data->SND_NXT, 0);
             chitcpd_update_tcp_state(si, entry, FIN_WAIT_1);
        }
    }
    else if (event == APPLICATION_RECEIVE)
    { 
      // ONLY THING WE DO HERE IS UPDATE THE TCP VARIABLES B/C SPACE IS NOW AVAILABLE IN THE BUFFER, DON'T YOU DARE DO ANYTHING ELSE
      circular_buffer_t *recv_buf = &(tcp_data->recv);
      tcp_data->RCV_WND = circular_buffer_available(recv_buf);
    }
    
        /* Your code goes here */
       //remember we usually don't IMMEDIATELY send a FIN packet, we have to wait until all outstanding data has been sent from our send buffer
       //closes the buffer, makes sure that nothing else can be written to it, but DOES need to send our stuff off before we send the FIN
      //keep this data either in a packet queue or storing it again in another buffer, the latter of which Borja highly recommends and suggests -i.e. copy current contents of buffer to a sep buffer and then call send functions on that? probz
    if (event == APPLICATION_CLOSE)
    {
        tcp_data->closing = 1;
        circular_buffer_close(send_buf);
        //while(circular_buffer_count(recv_buf)>0) {
                    //SEND all the preceding SENDS       
        //}

        if(circular_buffer_count(send_buf) == 0) {
            chitcpd_tcp_send_packet(si, entry, NULL, 0, 1, 0, tcp_data->SND_NXT, 0);
            chitcpd_update_tcp_state(si, entry, FIN_WAIT_1);
        }
    }

    else if (event == TIMEOUT)
    {
      /* Your code goes here */
    }
    else
        chilog(WARNING, "In ESTABLISHED state, received unexpected event (%i).", event);

    return CHITCP_OK;
}

int chitcpd_tcp_state_handle_FIN_WAIT_1(serverinfo_t *si, chisocketentry_t *entry, tcp_event_type_t event)
{
    if (event == PACKET_ARRIVAL)
    {
      chitcpd_tcp_handle_packet(si, entry, 0);  
    }
    else if (event == TIMEOUT)
    {
      /* Your code goes here */
    }
    else
       chilog(WARNING, "In FIN_WAIT_1 state, received unexpected event (%i).", event);

    return CHITCP_OK;
}


int chitcpd_tcp_state_handle_FIN_WAIT_2(serverinfo_t *si, chisocketentry_t *entry, tcp_event_type_t event)
{
    if (event == PACKET_ARRIVAL)
    {
        chitcpd_tcp_handle_packet(si, entry, 0);
    }
    else if (event == TIMEOUT)
    {
      /* Your code goes here */
    }
    else
        chilog(WARNING, "In FIN_WAIT_2 state, received unexpected event (%i).", event);

    return CHITCP_OK;
}


int chitcpd_tcp_state_handle_CLOSE_WAIT(serverinfo_t *si, chisocketentry_t *entry, tcp_event_type_t event)
{
    tcp_data_t *tcp_data = &entry->socket_state.active.tcp_data;
    circular_buffer_t * send_buf = &tcp_data->send;
        if (event == APPLICATION_CLOSE)
    {
        tcp_data->closing = 1;
        circular_buffer_close(send_buf);
        //while(circular_buffer_count(recv_buf)>0) {
                    //SEND all the preceding SENDS       
        //}

        if(circular_buffer_count(send_buf) == 0) {
            chitcpd_tcp_send_packet(si, entry, NULL, 0, 1, 0, tcp_data->SND_NXT, 0);
            chitcpd_update_tcp_state(si, entry, CLOSING);
        }
    }
    else if (event == PACKET_ARRIVAL)
    {
         chitcpd_tcp_handle_packet(si, entry, 0);
        if( (circular_buffer_count(send_buf) == 0) && (tcp_data->closing ==1) ) {
             chitcpd_tcp_send_packet(si, entry, NULL, 0, 1, 0, tcp_data->SND_NXT, 0);
             chitcpd_update_tcp_state(si, entry, CLOSING);
        }
    }
    else if (event == TIMEOUT)
    {
      /* Your code goes here */
    }
    else
       chilog(WARNING, "In CLOSE_WAIT state, received unexpected event (%i).", event);


    return CHITCP_OK;
}


int chitcpd_tcp_state_handle_CLOSING(serverinfo_t *si, chisocketentry_t *entry, tcp_event_type_t event) //either add tcp_data_free(&entry->socket_state.active.tcp_data;) here or in the handle packet function - dont think we have to clear anything else
{
    if (event == PACKET_ARRIVAL)
    {
         chitcpd_tcp_handle_packet(si, entry, 0);
    }
    else if (event == TIMEOUT)
    {
      /* Your code goes here */
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


int chitcpd_tcp_state_handle_LAST_ACK(serverinfo_t *si, chisocketentry_t *entry, tcp_event_type_t event) //either add tcp_data_free(&entry->socket_state.active.tcp_data;) here or in the handle packet function
{
    if (event == PACKET_ARRIVAL)
    {
         chitcpd_tcp_handle_packet(si, entry, 0);
    }
    else
       chilog(WARNING, "In LAST_ACK state, received unexpected event (%i).", event);

    return CHITCP_OK;
}

/*                                                           */
/*     Any additional functions you need should go here      */
/*                                                           */
