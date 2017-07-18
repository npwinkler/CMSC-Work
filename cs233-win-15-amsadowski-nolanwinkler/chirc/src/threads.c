/* 
 * Aidan Sadowski and Nolan Winkler
 * 
 * CSMC 23300 / 33300 - Networks and Distributed Systems
 * 
 * functions we had to add because of the use of threads and simclist
 * 
 */


#include "threads.h"
#include "simclist.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

/*Our custom seeker function to help in finding user structs by their socket # */ 
int seeker (const void * el, const void *key) {
    const struct ClientData * cli = (struct ClientData *) el;
    if (cli->ourSocket == *(int *)key)
        return 1;
    return 0;
}
/*
 * compare clients by nick
 * 
 * this function compares two clients:
 * <0: elOne later alphabetically than elTwo
 * 0: a equivalent to b
 * >0: elTwo later alphabetically than elOne
 */

int comparator (const void * elOne, const void * elTwo) {
    const struct ClientData * cliOne = (struct ClientData *) elOne;
    const struct ClientData * cliTwo = (struct ClientData *) elTwo;
    /*char nickOne[9];
    strcpy(nickOne,cliOne->nick);
    char nickTwo[9];
    nickTwo = cliTwo->nick;*/
    return strcmp(cliTwo->nick, cliOne->nick);
}

struct ClientData * findUser(char * nick, const list_t * cliList)
{
  struct ClientData * temp = (struct ClientData *) calloc(1, sizeof(struct ClientData));
  strcpy(temp->nick,nick);
      pthread_mutex_lock(&listLock);
  int position = list_locate(cliList, temp);
    if(position == -1) 
    {
        pthread_mutex_unlock(&listLock);
        return NULL;
    }
    else 
    {
        struct ClientData * desired = (struct ClientData *)list_get_at(cliList, position);
        pthread_mutex_unlock(&listLock);
        return desired;
    }
}

//FIIIIINE Mom, added a function to make a user so things are nicer
struct ClientData * makeClient(int IDSocket) {
  	
    struct ClientData * myData = (struct ClientData *) calloc(1, sizeof(struct ClientData));
       
    if(!myData)
    {
        perror("Calloc() failed.");
	      return NULL;
	  }

/* Set up the user struct */
	  myData->nick[0] = '\0'; //no nick yet
	  myData->user[0] = '\0'; //no user yet
      myData->ourSocket = IDSocket; //so we can find you
	  myData->registered = 0;//set to 1 when have good user and nicks
	  pthread_mutex_init(&myData->ourLock, NULL);
	  myData->userModes[0] = '\0';
      myData->away[0] = '\0';
      list_init(&myData->userInChannelList);
      pthread_mutex_init(&myData->UICListLock, NULL);

    return myData;
}
