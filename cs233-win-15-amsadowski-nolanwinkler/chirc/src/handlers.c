/* 
 * Aidan Sadowski and Nolan Winkler
 * 
 * CMSC 23300
 * 
 * Hanlder functions for all message types
 *
 */

#include <stdio.h>
#include "simclist.h"
#include "threads.h"
#include <string.h>
#include <pthread.h>
#include "bstrlib.h"
#include "parse.h"
#include "handlers.h"
#include <stdlib.h>
#include "sockets.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include "channel.h"


/*Function to handle a NICK message to our client by updating the user struct associated with the socket # it's coming from to have the right nickname */
void handle_NICK(char** params, int socket)
{
  char msgbuf[512];
  char *nick;
  char *user;

  //read the nick we got in the command
  nick = params[1];

  //check if the nick is already taken
  struct ClientData * ifused = findUser(nick,&clientList);
  if(ifused != NULL){
    printf("nick was taken\n");
    sprintf(msgbuf, ":%s 433 * %s :Nickname is already in use\r\n", host, nick);
    sendMessage(msgbuf, socket);
    return;
  }

  //look in the list of users for the matching socket number
  pthread_mutex_lock(&listLock);
  struct ClientData * ourCli = (struct ClientData *)list_seek(&clientList, &socket);
  if(ourCli == NULL) {
    printf("Failing on list seek.\n"); 
  }
  //add in their nick
  strcpy(ourCli->nick, nick);
  user = ourCli->user;
  pthread_mutex_unlock(&listLock);

  //list_dump_file(&clientList, "oneuserlist.txt", NULL);

  //logging!
  printf("Client # %d now has nickname %s.\n", ourCli->ourSocket, nick);
  printf("Client # %d also has username %s.\n", ourCli->ourSocket, user);

  //check if they have registered both a nick and a user
  if(nick[0] != '\0' && user[0] != '\0'){
    ourCli->registered = 1; //yay
    sprintf(msgbuf,":%s 001 %s :Welcome to the Internet Relay Network %s!%s@localhost.localdomain\r\n", host, nick, nick, user);
    //welcome them
    sendMessage(msgbuf, ourCli->ourSocket);
    sprintf(msgbuf, ":%s 002 %s :Your host is %s, running version chirc-0.1\r\n", host, nick, host);
    sendMessage(msgbuf, ourCli->ourSocket);
    sprintf(msgbuf, ":%s 003 %s :This server was created 2012-01-02 13:30:04\r\n", host, nick);
    sendMessage(msgbuf, ourCli->ourSocket);
    sprintf(msgbuf, ":%s 004 %s %s chirc-0.1 ao mtov\r\n", host, nick, host);
    sendMessage(msgbuf, ourCli->ourSocket);
    handle_LUSERS(NULL, socket);
    handle_MOTD(NULL, socket);
  }

  return;
}


/*Function to handle a USER message to our client by updating the user struct associated with the socket # it's coming from to have the right username */
void handle_USER(char** params, int socket)
{
  char msgbuf[512];
  char *nick;
  char *user;
  char *realname;

  //read the username we got in the command
  user = params[1];
  realname = params[10];

  /*Find the data structure for the client associated with our socket # */
  pthread_mutex_lock(&listLock);
  struct ClientData * ourCli = (struct ClientData *)list_seek(&clientList, &socket);
  if(ourCli == NULL) {
    printf("Failing on list seek.\n"); 
  }

  //updated the username
  strcpy(ourCli->user, user);
  strcpy(ourCli->real, realname);
  nick = ourCli->nick;
  pthread_mutex_unlock(&listLock);
  
  list_dump_file(&clientList, "onenicklist.txt", NULL);

  //logging
  printf("Client # %d now has username %s.\n", ourCli->ourSocket, user);
  printf("Client # %d also has nickname %s.\n", ourCli->ourSocket, nick);

  //check if they have registered a nick and a user
  if(nick[0] != '\0' && user[0] != '\0'){
    ourCli->registered = 1;
    //welcome them
    sprintf(msgbuf,":%s 001 %s :Welcome to the Internet Relay Network %s!%s@localhost.localdomain\r\n", host, nick, nick, user);
    sendMessage(msgbuf, ourCli->ourSocket);
    sprintf(msgbuf, ":%s 002 %s :Your host is %s, running version chirc-0.1\r\n", host, nick, host);
    sendMessage(msgbuf, ourCli->ourSocket);
    sprintf(msgbuf, ":%s 003 %s :This server was created 2012-01-02 13:30:04\r\n", host, nick);
    sendMessage(msgbuf, ourCli->ourSocket);
    sprintf(msgbuf, ":%s 004 %s %s chirc-0.1 ao mtov\r\n", host, nick, host);
    sendMessage(msgbuf, ourCli->ourSocket);
    handle_LUSERS(NULL, socket);
    handle_MOTD(NULL, socket);
  }
  return;
}


/*Function to handle a QUIT message -> I assume we want to free the client's user struct */
void handle_QUIT(char** params, int socket)
{
  //find the nick of the quitter
  struct ClientData *srcUser = (struct ClientData *)list_seek(&clientList, &socket);
  list_delete(&clientList, srcUser); //delete the corresponding clientData struct in the list
  close(socket);
  return;
}

/* Fuction to handle a private message by sending it to the server who sends it to the socket # associated with the client name given in the private message */
void handle_PRIVMSG(char** params, int socket)
{
  char msgbuf[512];
  int destsocket;
  channel *chan;
  struct ClientData * srcUser;
  
  //get the destination and message from the command
  char *dest = params[1];
  char *msg = params[10];

  //find the nick of the source
  srcUser = (struct ClientData *)list_seek(&clientList, &socket);
  
  if(dest[0] == '#'){ //messaging a channel
    
    chan = (channel *) findChannel(dest);
    if(chan == NULL){
      sprintf(msgbuf, ":%s 401 %s %s :No such nick/channel\r\n", host, srcUser->nick, dest);
      sendMessage(msgbuf, socket);
      return;
    }

    //check if the user is in the channel
    if(isUserInChannel(socket, chan) == -1){
      sprintf(msgbuf, ":%s 404 %s %s :Cannot send to channel\r\n", host, srcUser->user, dest);
      sendMessage(msgbuf, socket);
      return;
    }
    
    sprintf(msgbuf, ":%s!%s@hostname PRIVMSG %s :%s\r\n", srcUser->nick, srcUser->user, dest, msg);
    printf("message was: %s\n", msgbuf);
    messageAllUsers(msgbuf, chan, socket);
    
  } else {//messaging a user
  
    //find the information of the destination
    struct ClientData * destUser = findUser(dest, &clientList);
    if(destUser == NULL){
      sprintf(msgbuf, ":%s 401 %s %s :No such nick/channel\r\n", host, srcUser->nick, dest);
      sendMessage(msgbuf, socket);
      return;
    }
    destsocket = destUser->ourSocket;
    
    //send the message
    sprintf(msgbuf, ":%s!%s@srchost PRIVMSG %s :%s\r\n", srcUser->nick, srcUser->user, dest, msg);
    sendMessage(msgbuf, destsocket);
  }
  return;
}


void handle_NOTICE(char** params, int socket)
{
  char msgbuf[512];
  int destsocket;
  channel *chan;
  struct ClientData *srcUser;
  
  //get the destination and message from the command
  char *dest = params[1];
  char *msg = params[10];

  srcUser = (struct ClientData *)list_seek(&clientList, &socket);
  
  if(dest[0] == '#'){//channel

    chan = (channel *) findChannel(dest);
    if(chan == NULL){
      //sprintf(msgbuf, ":%s 401 %s %s :No such nick/channel\r\n", host, srcUser->nick, dest);
      //sendMessage(msgbuf, socket);
      return;
    }

    //check if the user is in the channel
    if(isUserInChannel(socket, chan) == -1){
      //sprintf(msgbuf, ":%s 404 %s %s :Cannot send to channel\r\n", host, srcUser->user, dest);
      //sendMessage(msgbuf, socket);
      return;
    }

    sprintf(msgbuf, ":%s!%s@hostname NOTICE %s :%s\r\n", srcUser->nick, srcUser->user, dest, msg);
    printf("message was: %s\n", msgbuf);
    messageAllUsers(msgbuf, chan, socket);

  } else {    //user
  
    //find the information of the destination
    struct ClientData * destUser = findUser(dest, &clientList);
    if(destUser == NULL){
      return;
    }
    destsocket = destUser->ourSocket;
    
    //find the nick of the source
    struct ClientData *srcUser = (struct ClientData *)list_seek(&clientList, &socket);
    
    //send the message
    sprintf(msgbuf, ":%s!%s@sentuser NOTICE %s :%s\r\n", srcUser->nick, srcUser->user, dest, msg);
    sendMessage(msgbuf, destsocket);
    return;

  }
}


void handle_PING(char** params, int socket)
{
  char msgbuf[512];
  sprintf(msgbuf, ":%s PONG %s\r\n", host, host); 
  sendMessage(msgbuf, socket);
  return;
}


void handle_PONG(char** params, int socket)
{
  return;
}


void handle_MOTD(char** params, int socket)
{
  char msgbuf[512];
  FILE *file;
  char filebuf[501];
  char user[500];
  int len;
  char nick[10];

  //find the nick of recpient
  struct ClientData *srcUser = (struct ClientData *)list_seek(&clientList, &socket);
  strcpy(nick, srcUser->nick);
  
  file = fopen("./motd.txt", "r");
  if(file != NULL){
    //start send
    sprintf(msgbuf, ":%s 375 %s :- %s Message of the day - \r\n", host, nick, host);
    sendMessage(msgbuf, socket);

    //get one line at a time
    while(NULL != fgets(filebuf, 500, file)){
      //check if the last char is a newline
      len = strlen(filebuf);
      if(filebuf[len-1] == '\n')
	filebuf[len-1] = '\0';
      
      sprintf(msgbuf, ":%s 372 %s :- %s\r\n", host, nick, filebuf);
      sendMessage(msgbuf, socket);
    }

    //end send
    sprintf(msgbuf, ":%s 376 %s :End of MOTD command\r\n", host, nick);
    sendMessage(msgbuf, socket);
    fclose(file);
    
  } else {
    struct ClientData * ourCli = (struct ClientData *)list_seek(&clientList, &socket);
    strcpy(user, ourCli->user);
    sprintf(msgbuf, ":%s 422 %s :MOTD File is missing\r\n", host, user);
    sendMessage(msgbuf, socket);
    return;
  }

  /*//tokenize MOTD around newlines
  printf("file is: %s", filebuf);
  bmotd = bfromcstr(filebuf);
  bmotdtok = bsplit(bmotd, '\n');
  
  //send each part of the motd
  printf("qty %d\n", bmotdtok->qty);
  for(i=0; i<(bmotdtok->qtly); i++){
    curpart = bstraEntry(bmotdtok, i);
    sprintf(msgbuf, ":%s 372 %s\r\n", host, curpart);
    sendMessage(msgbuf, socket);
  }
  */
  return;
}


/* Function to return a list of users connected/in a channel */
void handle_LUSERS(char** params, int socket)
{
  char msgbuf[512];
  int numUsers = 0;
  int numUnregUsers = 0;
  
  pthread_mutex_lock(&listLock);
  struct ClientData *cur;
  struct ClientData *cli;
  char user[500];

  //iterate through the list of users, get the number that are registered and unregistered
  if(list_iterator_start(&clientList) ==0) 
   {
      printf("Error iterating the list.\n");
      return;
   }
   while(list_iterator_hasnext(&clientList))
   {
     cur = (struct ClientData *)list_iterator_next(&clientList);
     if(cur->registered){
       numUsers++;
     } else {
       numUnregUsers++;
     }
   }
   if(list_iterator_stop(&clientList) == 0)
   {
      printf("Error iterating the list.\n");
      return;
   }
   pthread_mutex_unlock(&listLock);

   //get the client we are sending this to
   cli = (struct ClientData *)list_seek(&clientList, &socket);
   strcpy(user, cli->user);

   //send the message
   sprintf(msgbuf, ":%s 251 %s :There are %d users and 0 services on 1 servers\r\n", host, user, numUsers);
  sendMessage(msgbuf, socket);   
  sprintf(msgbuf,":%s 252 %s 0 :operator(s) online\r\n", host, user);
  sendMessage(msgbuf, socket);
  sprintf(msgbuf, ":%s 253 %s %d :unknown connection(s)\r\n",host, user, numUnregUsers);
  sendMessage(msgbuf, socket);
  sprintf(msgbuf,":%s 254 %s 0 :channels formed\r\n",host, user);
  sendMessage(msgbuf, socket);
  sprintf(msgbuf,":%s 255 %s :I have %d clients and 1 servers\r\n",host, user, numUsers+numUnregUsers);
  sendMessage(msgbuf, socket);
  return;
}


void handle_WHOIS(char** params, int socket)
{
  char msgbuf[512];
  char *nick = params[1];
  char user[500];
  char realname[500];
  struct ClientData * vip = findUser(nick,&clientList); //the VIP who everyone wants to know about, for whom they keep walking around town asking WHOIS in all caps and demanding a reply
  struct ClientData * looker = (struct ClientData *)list_seek(&clientList, &socket);
  char lookernick[10];
  strcpy(lookernick, looker->nick);

  //check if the nick exists
  if(vip == NULL){
    sprintf(msgbuf, ":%s 401 %s %s :No such nick/channel\r\n", host, lookernick, nick);
    sendMessage(msgbuf, socket);
    return;
  }
  strcpy(user, vip->user);
  strcpy(realname, vip->real);

  //send the info
  sprintf(msgbuf, ":%s 311 %s %s %s %s * :%s\r\n", host, lookernick, nick, user, host, realname);
  sendMessage(msgbuf, socket);
  sprintf(msgbuf, ":%s 312 %s %s %s :The best server ever\r\n", host, lookernick, nick, host);
  sendMessage(msgbuf, socket);
  sprintf(msgbuf, ":%s 318 %s %s :End of WHOIS list\r\n", host, lookernick, nick);
  sendMessage(msgbuf, socket);
  
  return;
}


//doesn't currently handle when user is already in channel
void handle_JOIN(char** params, int socket){
  char *chanName = params[1];
  channel *chan;
  char *user;
  char *nick;
  struct ClientData *beingAdded;
  char msgbuf[515];
  int ifnew = 0;
  
  //search for user by socket
  //printf("searching for user\n");
  pthread_mutex_lock(&listLock);
  beingAdded = (struct ClientData *)list_seek(&clientList, &socket);
  printf("%s\n", beingAdded->nick);
  pthread_mutex_unlock(&listLock);
  user = beingAdded->user;
  nick = beingAdded->nick;
  
  //search for channel by name
  printf("searching for channel\n");
  chan = (channel *) findChannel(chanName);

  //if it doesn't exist, create the channel
  if(chan == NULL){
    chan = createChannel(chanName, 0, 0, "", socket);
    pthread_mutex_lock(&chanListLock);
    list_append(&channelList, chan);
    pthread_mutex_unlock(&chanListLock);
    ifnew = 1;
  }
  
  //add user to that channel
  printf("adding user to channel\n");
  //start by checking if user is already joined
  if(isUserInChannel(socket, chan) != -1){
    //printf("user is already in channel\n");
    return;
  } else {
    pairUserWithChannel(beingAdded, chan, ifnew, socket);
  }
  //send out the message
  printf("sending message\n");
  sprintf(msgbuf, ":%s!%s@hostname JOIN %s\r\n", nick, user, chanName); //needs user address
  sendMessage(msgbuf, socket);
  messageAllUsers(msgbuf, chan, socket);
  if((chan->topic)[0] != '\0'){
    sprintf(msgbuf, ":%s 332 %s %s :%s\r\n", host, nick, chanName, chan->topic);
    sendMessage(msgbuf, socket);
  }
  sprintf(msgbuf, ":%s!%s@hostname 353 %s = %s :nicknames eventually go here\r\n", nick, user, nick, chanName);
  sendMessage(msgbuf, socket);
  sprintf(msgbuf, ":%s!%s@hostname 366 %s %s :End of NAMES list\r\n", nick, user, nick, chanName);
  sendMessage(msgbuf, socket);
  return;
}

void handle_PART(char ** params, int socket) {
  char *chanName = params[1];
  char *msg = params[10];
  char msgbuf[512];
  channel *chan;
  char *user;
  char *nick;
  struct ClientData *userstruct;
  
  //printf("msg: %s\n", msg);

  //search for user by socket
  printf("searching for user\n");
  pthread_mutex_lock(&listLock);
  userstruct = (struct ClientData *)list_seek(&clientList, &socket);
  pthread_mutex_unlock(&listLock);
  user = userstruct->user;
  nick = userstruct->nick;
  
  //search for channel by name
  printf("searching for channel\n");
  chan = (channel *) findChannel(chanName);
  
  //if channel doesn't exist
  if(chan == NULL){
    sprintf(msgbuf, ":%s 403 %s %s :No such channel\r\n", host, nick, chanName);
    sendMessage(msgbuf, socket);
    return;
  }

  //if user is not in channel
  printf("checking if user is in channel\n");
  if(isUserInChannel(socket, chan) == -1){
    sprintf(msgbuf, ":%s 442 %s %s :You're not on that channel\r\n", host, nick, chanName);
    sendMessage(msgbuf, socket);
    return;
  }

  //check if they have a parting message
  if(msg[0] == '\0'){
    //msg = nick;
    sprintf(msgbuf, ":%s!%s@hostname PART %s\r\n", nick, user, chanName);
    sendMessage(msgbuf, socket);
    messageAllUsers(msgbuf, chan, socket);
  } else {
    sprintf(msgbuf, ":%s!%s@hostname PART %s :%s\r\n", nick, user, chanName, msg);
    sendMessage(msgbuf, socket);
    messageAllUsers(msgbuf, chan, socket);
  }
  //do the deed
  printf("actually remvoing user\n");
  removeUser(socket, chan);
  
  return;
}

void handle_TOPIC(char** params, int socket) {
  char *chanName = params[1];
  char *msg = params[10];
  char *colonflag = params[9];
  char msgbuf[512];
  channel *chan;
  char *user;
  char *nick;
  struct ClientData *userstruct;
  char *newtopic;
  int chandneflag = 0;
  
  //search for user by socket
  //printf("searching for user\n");
  pthread_mutex_lock(&listLock);
  userstruct = (struct ClientData *)list_seek(&clientList, &socket);
  pthread_mutex_unlock(&listLock);
  user = userstruct->user;
  nick = userstruct->nick;

  //search for channel by name
  //printf("searching for channel\n");
  chan = (channel *) findChannel(chanName);

  //if channel doesn't exist
  if(chan == NULL){
    chandneflag = 1;
  }

  //if user is not in channel
  //printf("checking if user is in channel\n");
  if(chandneflag || (isUserInChannel(socket, chan) == -1)){
    sprintf(msgbuf, ":%s 442 %s %s :You're not on that channel\r\n", host, nick, chanName);
    sendMessage(msgbuf, socket);
    return;
  }

  //check what the User wants to do, check the topic or change it?
  if(colonflag[0] == 'n'){
    if((chan->topic)[0] != '\0'){
      sprintf(msgbuf, ":%s 332 %s %s :%s\r\n", host, nick, chanName, chan->topic);
      sendMessage(msgbuf, socket);
    } else {
      sprintf(msgbuf, ":%s 331 %s %s :No topic is set\r\n", host, nick, chanName);
      sendMessage(msgbuf, socket);
    }
  } else {
    free(chan->topic);
    newtopic = strdup(msg);
    chan->topic = newtopic;
    sprintf(msgbuf, ":%s!%s@localhost.localdomain TOPIC %s :%s\r\n", nick, user, chanName, chan->topic);
    sendMessage(msgbuf, socket);
    messageAllUsers(msgbuf, chan, socket);
  }
  
  return;
}

void handle_MODE(char **params, int socket) {
    //only should really care about -o for user and channel stuff
    char * NickorChan = params[1];
    char msgbuf[512];
    pthread_mutex_lock(&listLock);
    struct ClientData * ourCli = (struct ClientData *)list_seek(&clientList, &socket);
    pthread_mutex_unlock(&listLock);
    if(ourCli == NULL) {
            printf("Failing on list seek.\n"); 
    }
    char * name = ourCli->nick;
    char * user = ourCli->user;

    if(NickorChan[0] == '#') { //channel MODE command or member status MODE command
            channel * theChan = findChannel(NickorChan);
            if(theChan==NULL) { //not a channel
               sprintf(msgbuf, ":%s 403 %s %s :No such channel\r\n", host, name, NickorChan);
               sendMessage(msgbuf, socket);  
               return;
            }
            if(params[2]==NULL) { //no mode- print out our modes
                   sprintf(msgbuf, ":%s 324 %s %s +%s\r\n", host, name, NickorChan, theChan->chanModes);
                   sendMessage(msgbuf, socket);  
            }
            else { //we have a mode, but dont know if channel or user status
                    char * modeStr = params[2];
                    char PoM = modeStr[0];
                    char mode = modeStr[1];
                    //CHECK IF CHANOP OR SERVEROP
                    int position = isUserInChannel(socket, theChan);
                    pthread_mutex_lock(&(theChan->UICListLock));
                    userInChannel * theUIC = list_get_at(&(theChan->userInChannelList), position);
                    pthread_mutex_unlock(&(theChan->UICListLock));
                    int hasCOp = hasMode(theUIC->userChanModes, 'O');
                    int hasSOp = hasMode(ourCli->userModes, 'o');                            
                            
                    if(params[3]==NULL) { // Just a channel command
                            if( (mode != 'm') & (mode != 't') ) {
                                sprintf(msgbuf, ":%s 472 %s %c :is unknown mode char to me for %s\r\n", host, name, mode, theChan->name);
                                sendMessage(msgbuf, socket);
                                return;
                            }                                    
                            if(hasCOp || hasSOp) { //case tree on if we set or delete the mode
                                    if(PoM == '-') {
                                        removeMode(theChan->chanModes, mode);
                                        }
                                    else {//PoM == '+'
                                        setMode(theChan->chanModes, mode);
                                    }
                                        sprintf(msgbuf, ":%s!%s@localhost.localdomain MODE %s %c%c\r\n", name, user, NickorChan, PoM, mode);
                                        messageAllUsers(msgbuf, theChan, socket);
                                        sendMessage(msgbuf, socket);
                                        return;    
                            }
                            else {//not an op
                                sprintf(msgbuf, ":%s 482 %s %s :You're not channel operator\r\n", host, name, theChan->name);
                                sendMessage(msgbuf, socket);
                                return;
                            }
                    }
                    else {  //It's a member status command
                            char * dude = params[3];
                            if( (mode != 'v') & (mode != 'o') ) {
                                sprintf(msgbuf, ":%s 472 %s %c :is unknown mode char to me for %s\r\n", host, name, mode, theChan->name);
                                sendMessage(msgbuf, socket);
                                return;
                            }
                            else { // valid mode, check if operator
                                if(hasCOp || hasSOp) { //case tree on if we set or delete the mode
                                    //check if dude exists and is in the channel
                                    struct ClientData * userDude = findUser(dude, &clientList);
                                    if(userDude == NULL){
                                        printf("We just don't find the user in our server.\n");
                                        sprintf(msgbuf, ":%s 441 %s %s %s :They aren't on that channel\r\n", host, name, dude, theChan->name);
                                        sendMessage(msgbuf, socket);
                                        return;
                                    }
                                    int dudePos = isUserInChannel(userDude->ourSocket, theChan);
                                    if(dudePos == -1) {
                                        printf("We don't find the user in the channel.\n");
                                        sprintf(msgbuf, ":%s 441 %s %s %s :They aren't on that channel\r\n", host, name, dude, theChan->name);
                                        sendMessage(msgbuf, socket);
                                        return;
                                    }
                                    else { //wow, dude's actually in the channel way to go!
                                            pthread_mutex_lock(&(theChan->UICListLock));
                                            userInChannel * dudeUIC = list_get_at(&theChan->userInChannelList, dudePos);
                                            pthread_mutex_unlock(&(theChan->UICListLock));
                                            if(PoM == '-') {
                                                removeMode(dudeUIC->userChanModes, mode);
                                            }
                                            else {//PoM == '+'
                                                setMode(dudeUIC->userChanModes, mode);
                                            }
                                                //messageAllUsers(the message that came in, theChan); would we strcat on params or what?
                                                return;    
                                    }
                                }
                                else {//not an op
                                    sprintf(msgbuf, ":%s 482 %s %s :You're not channel operator\r\n", host, name, theChan->name);
                                    sendMessage(msgbuf, socket);
                                    return;
                                }
                            }   
                    }
                
            }
    }

    else { //NickorChan is a user, user MODE command
            if(strcmp(NickorChan, ourCli->nick) != 0) {
                       sprintf(msgbuf, ":%s 502 %s :Cannot change mode for other users\r\n", host, name);
                       sendMessage(msgbuf, socket);  
                       return; 
            }

            if(params[2]==NULL) { //no mode- print out our modes -> apparently should be the no such channel thing?
                   /*sprintf(msgbuf, ":%s MODE %s :%c%c\r\n", NickorChan, NickorChan, PoM, mode);
                   sendMessage(msgbuf, socket);*/
                   sprintf(msgbuf, ":%s 403 %s %s :No such channel\r\n", host, name, NickorChan);
                   sendMessage(msgbuf, socket);
                   return;
            }
            else { //we have a mode
                    char * modeStr = params[2];
                    char PoM = modeStr[0];
                    char mode = modeStr[1];
                if(mode == 'o') { //if PoM == +, ignore it must use OPER
                        if (PoM == '-') {
                            removeMode(ourCli->userModes, mode);
                            sprintf(msgbuf, ":%s!%s@localhost.localdomain MODE %s :%c%c\r\n", user, name, name, PoM, mode);
                            sendMessage(msgbuf, socket);
                        }
                        return;
                }

                else if (mode == 'a') { 
                        return;
                }//ignore it completely only AWAY works

                else {
                        sprintf(msgbuf, ":%s 501 %s :Unknown MODE flag\r\n", host, name);
                        sendMessage(msgbuf, socket);
                        return;
                }
            }
    }
    printf("Apparently there's a mode I don't deal with?\n");
}

void handle_AWAY(char **params, int socket) {
    char msgbuf[512];
    pthread_mutex_lock(&listLock);
    struct ClientData * ourCli = (struct ClientData *)list_seek(&clientList, &socket);
    pthread_mutex_unlock(&listLock);
    if(ourCli == NULL) {
            printf("Failing on list seek.\n"); 
    }
    if(params[1] == NULL) { //remove away status
           removeMode(ourCli->userModes, 'a');
           sprintf(msgbuf, ":%s 305 * %s :You are no longer marked as being away\r\n", host, ourCli->nick);
           sendMessage(msgbuf, socket);
           return;
    }
    else { //params[1] is the away message
          setMode(ourCli->userModes, 'a');
          strcpy(ourCli->away, params[1]);
          sprintf(msgbuf, ":%s 306 * %s :You have been marked as being away\r\n", host, ourCli->nick);
          sendMessage(msgbuf, socket);
          return;
    }
}

void handle_NAMES(char **params, int socket){
    char msgbuf[512];
    char * chanName;
    channel * chan;
    userInChannel * currUIC;
    char smallbuf[25];    
    if(params[1] == NULL) { //RPL_NAMREPLY for everything
        pthread_mutex_lock(&chanListLock);
        if(list_iterator_start(&channelList) == 0) {
               printf("Error iterating the list.\n");
               return;
        }
        while(list_iterator_hasnext(&channelList)) {
            chan = (channel *)list_iterator_next(&channelList);
            sprintf(msgbuf, ":%s 353 = %s :", host, chan->name);
            pthread_mutex_lock(&(chan->UICListLock));
            if(list_iterator_start(&(chan->userInChannelList)) == 0) {
                   printf("Error iterating the list.\n");
                   return;
            }
            while(list_iterator_hasnext(&(chan->userInChannelList))) {
                    currUIC = (userInChannel *)list_iterator_next(&chan->userInChannelList);
                    if(hasMode(currUIC->userChanModes, 'O')) {
                        sprintf(smallbuf, "@%s", (currUIC->user)->nick);
                        strcat(msgbuf, smallbuf);
                    }
                    else if(hasMode(currUIC->userChanModes, 'v')) {
                        sprintf(smallbuf, "+%s", (currUIC->user)->nick);
                        strcat(msgbuf, smallbuf);
                    }
                    else {
                        sprintf(smallbuf, "+%s", (currUIC->user)->nick);
                        strcat(msgbuf, smallbuf);                    
                    }
           }

           if(list_iterator_stop(&(chan->userInChannelList)) == 0)
           {
              printf("Error iterating the list.\n");
              return;
           }
           pthread_mutex_unlock(&(chan->UICListLock));
           sendMessage(msgbuf, socket);
           sprintf(msgbuf, ":%s 366 %s :End of NAMES list", host, chan->name);
           sendMessage(msgbuf, socket);
        }
        if(list_iterator_stop(&(channelList)) == 0)
        {
              printf("Error iterating the list.\n");
              return;
        }
        pthread_mutex_unlock(&chanListLock);        
    }

    else { //RPLY_NAMREPLY for the channel
        chanName = params[1];
        chan = (channel *) findChannel(chanName);
        sprintf(msgbuf, ":%s 353 = %s :", host, chan->name);
        pthread_mutex_lock(&(chan->UICListLock));
        if(list_iterator_start(&(chan->userInChannelList)) == 0) {
               printf("Error iterating the list.\n");
               return;
        }
        while(list_iterator_hasnext(&(chan->userInChannelList))) {
                currUIC = (userInChannel *)list_iterator_next(&chan->userInChannelList);
                if(hasMode(currUIC->userChanModes, 'O')) {
                        sprintf(smallbuf, "@%s", (currUIC->user)->nick);
                        strcat(msgbuf, smallbuf);
                    }
                    else if(hasMode(currUIC->userChanModes, 'v')) {
                        sprintf(smallbuf, "+%s", (currUIC->user)->nick);
                        strcat(msgbuf, smallbuf);
                    }
                    else {
                        sprintf(smallbuf, "+%s", (currUIC->user)->nick);
                        strcat(msgbuf, smallbuf);                    
                    }
        }
           if(list_iterator_stop(&(chan->userInChannelList)) == 0)
   {
      printf("Error iterating the list.\n");
      return;
   }
        pthread_mutex_unlock(&(chan->UICListLock));
        sendMessage(msgbuf, socket);
        sprintf(msgbuf, ":%s 366 %s :End of NAMES list", host, chan->name);
        sendMessage(msgbuf, socket);
    }
}

void handle_LIST(char **params, int socket){
  printf("at the top of handle_list, plenty of time to fill the printf buffer and send a message\n");
  char msgbuf[512];
  char* nick;
  char *chanName = params[1];
  channel* chan;
  
  struct ClientData * ourCli = (struct ClientData *)list_seek(&clientList, &socket);
  nick = ourCli->nick;

  //printf("Ahfksadkfhkwelkheiowhrihfksadnfnkahseihiofhiohdsfaskdnfkneifhihdisjfkalsdklfjksdjfij\n");
  //printf("chan null is :%d\n", chanName == NULL);
  //printf("shfksadkfhkwelkheiowhrihfksadnfnkahseihiofhiohdsfaskdnfkneifhihdisjfkalsdklfjksdjfij\n");
  if(chanName != NULL){
    chan = (channel *) findChannel(chanName);

    //if channel doesn't exist
    if(chan == NULL){
      return;
    }

    sprintf(msgbuf, ":%s 322 %s %s %d :%s\r\n", host, nick, chanName, chan->nusers, chan->topic); //add stuff
    sendMessage(msgbuf, socket);
    sprintf(msgbuf, ":%s 323 %s :End of LIST\r\n", host, nick);
    sendMessage(msgbuf, socket);
    
  } else {
  
    pthread_mutex_lock(&chanListLock);
    if(list_iterator_start(&channelList) == 0){
      printf("could not iterate list\n");
      return;
    }
    printf("starting channel iterator\n");
    while(list_iterator_hasnext(&channelList)){
      chan = (channel *) list_iterator_next(&channelList);
      printf("sending channel info\n");
      sprintf(msgbuf, ":%s 322 %s %s %d :%s\r\n", host, nick, chan->name, chan->nusers, chan->topic); //add stuff
      sendMessage(msgbuf, socket);  
    }

    list_iterator_stop(&channelList);
    pthread_mutex_unlock(&chanListLock);

    sprintf(msgbuf, ":%s 323 %s :End of LIST\r\n", host, nick);
    sendMessage(msgbuf, socket);
  }
  
  
  return;
}

void handle_WHO(char **params, int socket) {
    return;
}

 
void handle_OPER(char **params, int socket) {
    char * name = params[1];
    char * password = params[2];
    char msgbuf[512];
    pthread_mutex_lock(&listLock);
    struct ClientData * ourCli = (struct ClientData *)list_seek(&clientList, &socket);
    pthread_mutex_unlock(&listLock);
    if(ourCli == NULL) {
            printf("Failing on list seek.\n");
    }

    if(strlen(name)<1) {
        //expect a user name but should ignore it
        return;
    }
        //this needs to be updated to be the -o command which is passwd
    if(strcmp(passwd, password) != 0) {
        //ERR_PASSWDMISMATCH
        sprintf(msgbuf, ":%s 464 %s :Password incorrect\r\n", host, ourCli->nick);
        sendMessage(msgbuf, socket);
    }
    else {//update their mode
            if(setMode(ourCli->userModes, 'o')>0) {   //RLP_YOUREOPER     
                 sprintf(msgbuf, ":%s 381 %s :You are now an IRC operator\r\n", host, ourCli->nick);
                 sendMessage(msgbuf, socket);
                 sprintf(msgbuf, ":%s MODE %s :+s\r\n", host, ourCli->nick); //assuming we can trust 3.1.5
            }
    }
    return;
}
