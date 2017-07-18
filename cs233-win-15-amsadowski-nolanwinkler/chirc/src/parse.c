/* 
 * Aidan Sadowski and Nolan Winkler
 * 
 * CSMC 23300 / 33300 - Networks and Distributed Systems
 * 
 * functions to parse incomming commands and reply to clients
 * 
 */

#include "handlers.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include "bstrlib.h"
#include <pthread.h>
#include "threads.h"
#include "sockets.h"
#include <stdlib.h>

typedef void (*handler_function)(char** params, int socket);

struct handler_entry
{
char *entry_name;
handler_function func;
};

//dispatch table
struct handler_entry handlers[] = {
{"NICK", handle_NICK},
{"USER", handle_USER},
{"QUIT", handle_QUIT},
{"PRIVMSG", handle_PRIVMSG},
{"NOTICE", handle_NOTICE},
{"PING", handle_PING},
{"PONG", handle_PONG},
{"MOTD", handle_MOTD},
{"LUSERS", handle_LUSERS},
{"WHOIS", handle_WHOIS},
{"PART", handle_PART},
{"TOPIC", handle_TOPIC},
{"MODE", handle_MODE},
{"AWAY", handle_AWAY},
{"NAMES", handle_NAMES},
{"LIST", handle_LIST},
{"WHO", handle_WHO},
{"JOIN", handle_JOIN},
{"OPER", handle_OPER},
};

int num_handlers = sizeof(handlers) / sizeof(struct handler_entry);

//function to send a message and do error checking + locking
void sendMessage(char *message, int socket)
{
  pthread_mutex_lock(&intlock);
  if(betterSend(socket, message, strlen(message)) < 0){
    perror("Socket send() failed");
  }
  pthread_mutex_unlock(&intlock);
}

//utility function to pull a bstring out of a bstring list by index
char *bstraEntry(struct bstrList *tokenArray, int pos)
{
  int size = tokenArray->qty;
  if(pos >= size)
    return NULL;
  char *entry = bstr2cstr(tokenArray->entry[pos], '\0');
  return entry;
}
  
//tokenize and follow case tree based on message type
void parse(bstring message, int socket)
{
  struct bstrList* tokenArray;
  struct bstrList* colonArray;
  char *charTokenArray[11];
  char reply[512]; //for message type not known error
  int i,j, prefixFirst;
  char *flag = (char*) malloc (sizeof(char));

  //split around the colon to extract the message - if it exists
  prefixFirst = binstr(message, 0, bfromcstr(":"));
  if(prefixFirst != BSTR_ERR){
    colonArray = bsplit(message, ':');
    message = bfromcstr(bstraEntry(colonArray,0)); //everything before the colon
    charTokenArray[10] = bstraEntry(colonArray,1); //everything after the colon
    flag[0] = 'y';
    charTokenArray[9] = flag;
  } else {
    char *empty = (char *)malloc(sizeof(char));
    empty[0] = '\0';
    charTokenArray[10] = empty;
    flag[0] = 'n';
    charTokenArray[9] = flag;
  }

  //tokenize around spaces - only works for well formed messages
  /* MIGHT WANT TO FIX FOR ROBUSTNESS STUFF IN 1C */
  tokenArray = bsplit(message, ' ');
  
  //convert bstrings to cstrings, only needed bstring for easy tokenization
  for(j=0; j<8; j++) 
  {
    charTokenArray[j] = bstraEntry(tokenArray, j);
  }
  //case tree around first token
  for(i=0; i<num_handlers; i++)
  {
    if(!strcmp(handlers[i].entry_name, charTokenArray[0]))
    {
      handlers[i].func(charTokenArray, socket); //dispatch'd
      break;
    }
  }

  if(i == num_handlers)
  {
    //message type not found error
    struct ClientData * cliData = (struct ClientData *)list_seek(&clientList, &socket);
    char user[500];
    strcpy(user, cliData->user);
    sprintf(reply, ":%s 421 %s %s :Unknown command\r\n", host, user, charTokenArray[0]);
    sendMessage(reply, socket);
  }
    
}
      
 
