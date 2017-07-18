/*
 * Aidan Sadowski and Nolan Winkler
 * 
 * CMSC 23300
 * 
 * c file for better socket functions
 *
 */
#include "sockets.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/* Send() wrapper function that, if possible, sends an entire message of size msgLen */
int betterSend(int socket, char * message, int msgLen)
{
	int sentSoFar = 0;
	int sentThisTime = 0;
	while(sentSoFar < msgLen) // stop when we've sent as many bytes as we want to
	{ /* send #bytes left to send from the message buffer starting at the position after the number of bytes we've sent so far */
		sentThisTime = send(socket, message+sentSoFar, msgLen-sentSoFar, 0); 
		if(sentThisTime == -1) //send failed
 		{
			return -1;
		}
		else sentSoFar = sentSoFar + sentThisTime; 	
	}
	return 0;
}


