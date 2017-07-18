/*
 * Aidan Sadowski and Nolan Winkler
 * 
 * CMSC 23300
 * 
 * header file for structs and functions we had to add due to use of pthread and simclist
 *
 */

#ifndef THREADS_H
#define THREADS_H

#include "simclist.h"

char host[100];

/* Our global user struct list and locks */
pthread_mutex_t listLock;
pthread_mutex_t intlock;
char *port;
char *passwd;

list_t clientList;

/* Our helper fun for finding user by socket # */
int seeker (const void * el, const void *key);
int comparator (const void * elOne, const void * elTwo);

/* Our user struct which keeps their socket #, nickname, username, and later list of friends and channels */
struct ClientData {
  int ourSocket;
  pthread_mutex_t ourLock;
  char nick[9];
  char user[500];
  int registered;
  char host[100];
  char service[100];
  char real[500];
  char away[100];
  char userModes[10];
  list_t userInChannelList;
  pthread_mutex_t UICListLock;
}; // should eventually rename this to be a client, would have to update a decent amount of code

typedef struct ClientData * Client;

struct ClientData * findUser(char * nick, const list_t * cliList);

struct ClientData * makeClient(int IDSocket); //could change this to give back a Client -> would have to update lots o' code

#endif



