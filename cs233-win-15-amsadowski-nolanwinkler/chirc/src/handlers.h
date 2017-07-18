#ifndef HANDLERS_H
#define HANDLERS_H

/*
 * Aidan Sadowski and Nolan Winkler
 * 
 * CMSC 23300
 * 
 * header file for message handler functions that update our clients based on their message type
 *
 */

void handle_NICK(char** params, int socket);

void handle_USER(char** params, int socket);

void handle_QUIT(char** params, int socket);
 
void handle_PRIVMSG(char** params, int socket);

void handle_NOTICE(char** params, int socket);

void handle_PING(char** params, int socket);

void handle_PONG(char** params, int socket);

void handle_MOTD(char** params, int socket);

void handle_LUSERS(char** params, int socket);

void handle_WHOIS(char** params, int socket);

void handle_PART(char ** params, int socket);

void handle_TOPIC(char** params, int socket);

void handle_MODE(char **params, int socket);

void handle_AWAY(char **params, int socket);

void handle_NAMES(char **params, int socket);

void handle_LIST(char **params, int socket);

void handle_WHO(char **params, int socket);

void handle_JOIN(char **params, int socket);

void handle_OPER(char **params, int socket);

#endif
