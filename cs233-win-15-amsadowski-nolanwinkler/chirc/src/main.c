/*
 * Aidan Sadowski and Nolan Winkler
 *
 *  CMSC 23300 / 33300 - Networks and Distributed Systems
 *
 *  main() code for chirc project
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "bstrlib.h"
#include <pthread.h>
#include "threads.h"
#include "parse.h"
#include "simclist.h"
#include "sockets.h"
#include "channel.h"

/* globals - note there are more in threads.h */

int nickMsg = 0;
int userMsg = 0;
char nick[9];
char user[17];
char reply[500];
char host[100]; 
// -> need to put in ourServer now so we can keep *passwd, I think
int bigbuffsize = 2000;
int smallbuffsize = 512;

/* thread functions */

void * serverListen(void * args);
void * handleClient (void * args);

/* Main function spawns the server's listening thread and sets up locks */

int main(int argc, char *argv[])
{
	//wholeServer ourServer;
	int opt;
	while ((opt = getopt(argc, argv, "p:o:h")) != -1)
		switch (opt)
      		{
      			case 'p':
				port = strdup(optarg); //ourServer.port
				break;
     			case 'o':
				passwd = strdup(optarg); //ourServer.passwd
				break;
      			default:
				printf("ERROR: Unknown option -%c\n", opt);
				exit(-1);
      		}

  		if (!passwd)
 		{
      			fprintf(stderr, "ERROR: You must specify an operator password\n");
      			exit(-1);
    		}

/* Initalizing our ClientList data structure, spawning the thread that listens for clients, initializing locks */

	list_init(&clientList);
	list_attributes_seeker(&clientList, seeker); //can seek by socket # to find the client that sent us the message we're dealing with
	list_attributes_comparator(&clientList, comparator); //can search by nickname to find the intended recipient of messages
	list_init(&channelList);
	list_attributes_seeker(&channelList, seekByName); //can use list_seek to get out a channel by its name
	list_attributes_comparator(&channelList, compareChannels); //can use position = list_locate(name) & list_get_at(position) to get out a channel by its name
	
	pthread_t server_thread;
	pthread_mutex_init(&listLock, NULL);
	pthread_mutex_init(&intlock, NULL);
    pthread_mutex_init(&chanListLock, NULL);

	if (pthread_create(&server_thread, NULL, serverListen, NULL) < 0)
	{
		perror("Could not create server thread");
		exit(-1);
	}

	/* When listening thread is done, we're done, reap it, & destroy locks */

	pthread_join(server_thread, NULL);

	pthread_mutex_destroy(&listLock);
	pthread_mutex_destroy(&intlock);
	pthread_exit(NULL);
	return 0;
}


/* Function for the thread spawned to listen for clients, accept them, and create new threads to service them */


void * serverListen(void * args) {

/* Set up socket structures, and things we need to pass into socket(), accept(), listen(), bind() */
	int yes = 1;
 	int listeningSocket;
	int handlingSocket;
	struct sockaddr_in serverAddr; //we don't need to do IPv6
	struct sockaddr_in * clientAddr; //shouldn't need to be in loop, we're good
	pthread_t clientThread;
	socklen_t sinSize = sizeof(struct sockaddr_in);
	//struct ArgsforClient * aForC;
	

  	memset(&serverAddr, 0, sizeof(serverAddr));

/*create listening socket */
  	listeningSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(listeningSocket == -1)
    	{
     		perror("Could not open socket");
      		exit(-1);
    	}

  	serverAddr.sin_family = AF_INET;
  	serverAddr.sin_port = htons(atoi(port));
  	serverAddr.sin_addr.s_addr = INADDR_ANY;

/* One place I could be messing up is in not using getaddrinfo() */

  	setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

/*bind the listening socket */
  	if(bind(listeningSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1)
    	{
     		perror("Socket bind() failed");
      		close(listeningSocket);
      		exit(-1);
    	}

/* listen on the listeningSocket */
  	if(listen(listeningSocket, 5) == -1)  //Borja says 5
	{
     		perror("Socket listen() failed");
      		close(listeningSocket);
      		exit(-1);
    	}

/* LISTENING SOCKET COMPLETELY SET UP NOW */

    gethostname(host, 100);
    //printf("The hostname of this computer is %s\n", host); 

  /* Listen for clients, when we get one make it its own thread, rinse and repeat */

	while(1) {
/* make a new address struct for this incoming client */
		clientAddr = malloc(sinSize);
		if(!clientAddr) {
			perror("Malloc() failed.");
			exit(EXIT_FAILURE);
		}
/* accept incoming client requests */
	  	if( (handlingSocket = accept(listeningSocket, (struct sockaddr *) clientAddr, &sinSize)) == -1) {
			perror("Socket accept() failed");
			free(clientAddr);
			continue;
      		}
		//else printf("Got a new connection on handlingSocket # %d\n", handlingSocket);

/*Make a new thread specific to that client to handle it */

		if (pthread_create(&clientThread, NULL, handleClient, (void*) &handlingSocket) != 0) 
		{
			perror("Could not create a worker thread");
			free(clientAddr);
			close(handlingSocket);
			close(listeningSocket);
			pthread_exit(NULL);
		}
	} //loop again

	pthread_exit(NULL);
	close(listeningSocket); //not 100% necessary
}

/*Function for each individual thread to service one client we've already connected to */
	
void * handleClient(void * args){
/* First, get out our socket # so we can talk to you */
	int mySocket;
	mySocket = *((int *) args);
	//printf("mySocket is %d\n", mySocket); client thread is set up
	pthread_detach(pthread_self());

/* Know which client # we are - maybe only useful for test
	pthread_mutex_lock (&thrNumLock);
	//int myNum = threadNum++;
	pthread_mutex_unlock (&thrNumLock); */

/*Create a user struct for this connection */
	
	/* replaced by makeClient(mySocket);
	
	struct ClientData * myData = (struct ClientData *) calloc(1, sizeof(struct ClientData));
        if(!myData) {
          perror("Calloc() failed.");
	  exit(EXIT_FAILURE);
	}

 Set up the user struct
	myData->nick[0] = '\0'; //no nick yet
	myData->user[0] = '\0'; //no user yet
    myData->ourSocket = mySocket; //so we can find you
	  myData->registered = 0;//set to 1 when have good user and nicks
	  pthread_mutex_init(&myData->ourLock);
*/

  struct ClientData * myData = makeClient(mySocket);
  if(!myData) {
    printf("Can't make a client.\n");
    exit(EXIT_FAILURE);
  }

//STILL NEED TO IMPLEMENT THIS
 /* char hbuf[100];
    char sbuf[100];
    struct sockaddr_in forhost;
    socklen_t len = sizeof(struct sockaddr_in);
    printf("Hbuf is %s, sbuf is %s", hbuf, sbuf);
    if(getnameinfo((struct sockaddr *)&forhost, len, hbuf, 100, sbuf, 100, 0)!=0)
    {
        printf("Error cannot find host name.");
    }
    //getpeername(myData->ourSocket, (struct sockaddr*) &forhost, &len);
	//socklen_t saSize = sizeof(struct sockaddr); 
	//printf("Our client is coming to use from host %s service %s\n", hbuf, sbuf);
	//printf("This user will be registered to socket number %d.\n", myData->ourSocket);
	*/

/* Add the user struct to our list */
	pthread_mutex_lock(&listLock);
	list_append(&clientList, myData);
	pthread_mutex_unlock(&listLock);
	


/* Initialize buffers for read() and parsing and set up variables for that logic */
	char bigBuffer[bigbuffsize+1];
  	char smallBuffer[smallbuffsize+1];
  	int nbytes;
   	bstring onemessage;
    	int j=0;
    	while(j<bigbuffsize){
      		bigBuffer[j] = '\0';
      		j++;
   	}
    	int isbackn = 0;
    	int isbackr = 0;
    	int iinbig = 0;
    	int iinsmall = 0;
    	char cur;

/* Read from the socket, move the data into big buffer, when we have a complete message put it into smallbuffer and then make it a bstring for easy parsing */

    	while(1) {
      		nbytes = recv(mySocket, bigBuffer, bigbuffsize, 0);
      		if(nbytes > 0){ // i.e. socket has not yet been closed - socket gets cloed in HANDLE QUIT
			//printf("bigBuffer prints %s\n", bigBuffer);
			for(iinbig = 0; iinbig < nbytes; iinbig++){ //look to see if we have "\r\n" in our buffer
	  			cur = bigBuffer[iinbig];
	  			smallBuffer[iinsmall] = cur;
	  			if(cur == '\r')
	    				isbackr = 1;
	  			if(cur == '\n')
	    				isbackn = 1;
	  			if(isbackr && isbackn){ //if we do, take everything up to and including that and treat it as one message
	    				//printf("we got the good\n");
	    				smallBuffer[iinsmall+1] = '\0';
	    				//printf("smallBuffer prints %s\n",smallBuffer);
	    				onemessage = bfromcstr(smallBuffer);
	    				//printf("onemessage is: %s and iinsmall is: %d\n", bstr2cstr(onemessage,'\0'), iinsmall);
	    				if(binstr(onemessage,0,bfromcstr("\r\n")) != BSTR_ERR){
	      					//printf("it was complete\n");
	      					onemessage = bmidstr(onemessage,0,blength(onemessage)-2); //remove /r/n from end of onemssage
	      					parse(onemessage, mySocket);
	      					iinsmall = -1;
	    				}
	    				isbackr = isbackn = 0;
	  			} //if we don't, go back to our big buffer, get the next data, and add it back onto the end of our little bufer
	  			iinsmall++;
			}
      		}
	}
   //can try going to the list, deleting the guy, and also free'ing some shit
   list_delete_at(&clientList, list_locate(&clientList, &mySocket));  
   pthread_exit(NULL);
}
