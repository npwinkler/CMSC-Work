/* 
 * Aidan Sadowski and Nolan Winkler
 * 
 * CMSC 23300
 * 
 * Header file for functions for channels and users in channels 
 *
 */

#include "simclist.h"
#include "bstrlib.h"
#include "parse.h"

list_t channelList;
pthread_mutex_t chanListLock;

typedef struct channel{
  char *name;
  //list_t users; deprecated due to other list
  //int mmode;
  //int tmode; both will be in modes
  char chanModes[10];
  char *topic;
  list_t userInChannelList;
  pthread_mutex_t UICListLock;
  int nusers;
} channel;

typedef struct userinchannel{
  channel * chan;
  struct ClientData *user;
  //int omode;
  //int vmode; both will be in modes
  char userChanModes[10];
} userInChannel;

void messageAllUsers(char *msg, channel *chan, int msger);

void addUser(int usersoc, channel *chan);

void removeUser(int usersoc, channel *chan);

int isUserInChannel(int usersoc, channel *chan);

//name and topic should be strdupped
//god is the user who created the channel and the first user placed in it
channel *createChannel(char *name, int mmode, int tmode, char *topic, int god);

void destroyChannel(channel *chan);

int seekBySocket(const void * el, const void * key);

int compareSockets(const void* userOne, const void * userTwo);

int seekByName(const void * el, const void * key);

int compareChannels(const void* channelOne, const void* channelTwo);

channel * findChannel(char *name);

int hasMode(char * modes, char mode);

int setMode(char * modes, char mode);

int removeMode(char * modes, char mode);

userInChannel * makeUserInChannel(struct ClientData * cli, channel * chan);

void addUserInChannel(channel *chan, userInChannel * joiner);//makes a UserInChannel for the person joining the channel & adds that struct to the channel's UserInChannelList

void addChanneltoUsersList(userInChannel * cliInChan, struct ClientData * cli);

void pairUserWithChannel(struct ClientData * cli, channel * chan, int ifCOp, int socket); //essentially replaced addUser in HANDLE_JOIN
