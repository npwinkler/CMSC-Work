/* 
 * Aidan Sadowski and Nolan Winkler
 * 
 * CMSC 2300
 *
 * utility functions for channels
 * 
 */

#include "channel.h"
#include <stdio.h>
#include <stdlib.h> 
#include "threads.h"
#include "simclist.h"
#include <pthread.h>

void messageAllUsers(char *msg, channel *chan, int msger){//in a channel -> also need to go through and check for the away modes type shit
  list_t userList = chan->userInChannelList;
  printf("%d\n", list_size(&userList));
  pthread_mutex_lock(&(chan->UICListLock));
  if(list_iterator_start(&userList) == 0){
    printf("could not iterate list\n");
    pthread_mutex_unlock(&(chan->UICListLock));
    return;
  }

  while(list_iterator_hasnext(&userList)){//U OF C, not U I C!!!!
    userInChannel * currUIC = (userInChannel *)list_iterator_next(&userList);
    struct ClientData * currUser = currUIC->user;
    if(currUser->ourSocket != msger) {//skip over guy sending the message
        if(!hasMode(currUser->userModes, 'a')) {
            int socket = currUser->ourSocket;
            printf("sent message to socket %d\n", socket);
            sendMessage(msg, socket);
        }
        else {//person is away
                  char msgbuf[512];
                  sprintf(msgbuf, ":%s 301 %s :%s\r\n", host, currUser->nick, currUser->away);
                  sendMessage(msgbuf, msger);
        }
    }
  }
  list_iterator_stop(&userList);
  pthread_mutex_unlock(&(chan->UICListLock));
  return;
}

//adds the identifier for a user - the socketnumber - to a channel
//NOLAN NOTE: Why are we doing this instead of just adding them as a userInChannel structure??
//Replaced by addUserInChannel & addChanneltoUsersList -> maybe write a combo function called addUser that just calls both
//i.e. pairUserWithChannel
/*void addUser(int usersoc, channel *chan){
  int *soc = (int *) malloc(sizeof(int));
  *soc = usersoc;
  printf("added user to list with socket %d\n", *soc);
  list_append(&(chan->users), soc);
  return;
}*/

/* returns -1 if not found, position in list otherwise */
int isUserInChannel(int usersoc, channel *chan){
  list_t userList = chan->userInChannelList;
  int found = 0;
  int socket;
  userInChannel * currUIC;
  struct ClientData * currUser;
  pthread_mutex_lock(&(chan->UICListLock));
  if(list_iterator_start(&userList) == 0){
    printf("could not iterate list\n");
    pthread_mutex_unlock(&(chan->UICListLock));
    return -1;
  }

  while(list_iterator_hasnext(&userList)){ //could just do the list_attribute_set seeker type stuff,homie but IDK if I want to change this stuff
    currUIC = (userInChannel *)list_iterator_next(&userList);
    currUser = currUIC->user;
    socket = currUser->ourSocket;
    if(socket == usersoc){
      list_iterator_stop(&userList);
      pthread_mutex_unlock(&(chan->UICListLock));
      //printf("user found status: %d\n", found);
      return found;
    }
    found++;
  }
  list_iterator_stop(&userList);
  pthread_mutex_unlock(&(chan->UICListLock));
  //printf("user found status: %d\n", found);
  return -1;
}


channel *createChannel(char *name, int mmode, int tmode, char *topic, int god){ //wanna pass something better than int for god
  channel *chan;

  if((chan = (channel *) malloc(sizeof(channel))) == NULL){
    printf("error mallocing space for channel\n");
    return NULL;
  }

  list_init(&(chan->userInChannelList));
  chan->name = strdup(name);
  chan->topic = strdup(topic);
  chan->chanModes[0] = '\0';
  chan->nusers = 0;
  pthread_mutex_init(&(chan->UICListLock), NULL);
  pthread_mutex_lock(&listLock);
  struct ClientData * godCli = (struct ClientData *)list_seek(&clientList, &god);
  pthread_mutex_unlock(&listLock);
  
  
  return chan;
}

void destroyChannel(channel *chan){
  printf("destroying channel\n");
  int position = list_locate(&channelList, chan);
  list_delete_at(&channelList, position);
  free(chan->name);
  free(chan->topic);
  list_destroy(&(chan->userInChannelList));
  return;
}

void removeUser(int usersoc, channel *chan){
  int pos = isUserInChannel(usersoc, chan);
  printf("position in user list is %d\n", pos);
  list_t * userList = &(chan->userInChannelList);
  int suc = list_delete_at(userList, pos);
  printf("list delete status was %d\n", suc);
  (chan->nusers)--;
  if(chan->nusers == 0)
    destroyChannel(chan);
  return;
}


//need to rewrite to be for userInChannel shit meaning we should probably just list init and stuff like that

/* finds users in the userlist by their socket # */
int seekBySocket(const void * el, const void * key) 
{
    const int * eleSock = (int *) el;
    if(*eleSock == *(int *)key)
        return 1;
    return 0;
}
/*
 * compare users in the userlist by socket #
 * 
 * this function compares two users:
 * <0: userOne's socket greater than userTwo
 * 0: socket # the same
 * >0: userTwo's socket # greater than userOne's
 */

int compareSockets(const void* userOne, const void * userTwo)
{
    const int * oneSock = (int *) userOne;
    const int * twoSock = (int *) userTwo;
    return (*oneSock < *twoSock) - (*oneSock > *twoSock);
}

/* Finds channels in the channellist by their name */
int seekByName(const void * el, const void * key)
{
    const channel * theChan = (channel *)el;
    const char * name = (char *)key;
    if(strcmp(name, theChan->name)==0)
        return 1;
    return 0;
}


/*

 * compare channels in the channellist by channel #
 * 
 * this function compares two users:
 * <0: userOne's socket greater than userTwo
 * 0: socket # the same
 * >0: userTwo's socket # greater than userOne's
 */

int compareChannels(const void* channelOne, const void* channelTwo)
{
    const channel * chanOne = (channel *)channelOne;
    const channel * chanTwo = (channel *)channelTwo;
    return strcmp(chanTwo->name, chanOne->name);
}

channel * findChannel(char * name) {
  channel * temp = createChannel(name, 0, 0, "tmp", 9999999);
  strcpy(temp->name, name);
  printf("locating\n");
  int position = list_locate(&channelList, temp);
  if(position == -1) 
    {
      return NULL;
    }
  else 
    {
      printf("getting channel at position\n");
      channel * desired = (channel *)list_get_at(&channelList, position);
      return desired;
    }
}



int hasMode(char * modes, char mode) {
  int i =0;
  for(i=0;i<strlen(modes);i++) {
      if(modes[i] == mode) {
          printf("We had the mode %c at position %d\n", mode, i);
          return 1;
      }
  }
  return 0;
}

//not sure why int -> also not dealing with case of if we already have 10 modes
int setMode(char * modes, char mode) {
  int i = 0;
  for(i=0;i<strlen(modes);i++) {
      if(modes[i] == mode) {//mode already set
          printf("We had the mode %c at position %d so we shouldn't need to set it\n", mode, i);
          return 1;
      }
  }
  printf("We didn't have the mode %c so we're going to add it in\n", mode);
  modes[i]=mode;
  modes[i+1]='\0';
  return 1;
}
  
//also confusingly an int
int removeMode(char * modes, char mode) {
  int i, j = 0;
  int origLen = strlen(modes);
  for(i=0; i<origLen;i++) {
      if(modes[i] == mode) { //actually need to take it out
          printf("We had the mode %c at position %d so we should delete it\n", mode, i);
          for(j=i; j<origLen; j++) { //go thru rest of array, moving shit over
              modes[j] = modes[j+1];
          }
          printf("We deleted mode %c so now mode string is %s\n", mode, modes);
          break;
      }//else just don't touch the array
  }
  return 1;
}

userInChannel * makeUserInChannel(struct ClientData * cli, channel * chan) {
    userInChannel * joined = (userInChannel *)malloc(sizeof(userInChannel));
    joined->chan = chan;
    joined->user = cli;
    return joined;
}
  

void addUserInChannel(channel *chan, userInChannel * joiner) {
    pthread_mutex_lock(&(chan->UICListLock));
    list_append(&chan->userInChannelList, joiner); //add joiner's userInChannel struct to the channel's list of these
    pthread_mutex_unlock(&(chan->UICListLock));
}

    
void addChanneltoUsersList(userInChannel * cliInChan, struct ClientData * cli) {
    pthread_mutex_lock(&(cli->UICListLock));
    list_append(&cli->userInChannelList, cliInChan);
    pthread_mutex_unlock(&(cli->UICListLock));
}

//should definitely have a hybrid function of these two
//first make a new channel or if the channel we want to connect to already exists,
void pairUserWithChannel(struct ClientData * cli, channel * chan, int ifCOp, int socket) {
    userInChannel * use = makeUserInChannel(cli, chan);
    (chan->nusers)++;
    addUserInChannel(chan, use);
    addChanneltoUsersList(use, cli);
    if(ifCOp) {
                    /*int position = isUserInChannel(socket, chan);
                    pthread_mutex_lock(&(chan->UICListLock));
                    userInChannel * godUIC = list_get_at(&(chan->userInChannelList), position);
                    pthread_mutex_unlock(&(chan->UICListLock));
                    setMode(godUIC->userChanModes, 'O');*/
                    setMode(use->userChanModes, 'O');
    }
}
