#ifndef PARSE_H
#define PARSE_H

/* 
 * Aidan Sadowski and Nolan Winkler
 * 
 * CMSC 23300
 * 
 * header file for message parsing functions
 *
 */

void sendMessage(char *message, int socket);

char *bstraEntry(struct bstrList *tokenArray, int pos);

/*Takes a message from a given client and sends it to the message handler/dispatch table */
void parse(bstring message, int socket);

#endif
