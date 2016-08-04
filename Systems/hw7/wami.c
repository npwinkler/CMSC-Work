/*
** Written by Gordon Kindlmann for cs154-2014 HW7
** based in part on http://goo.gl/695X1P
*/

/* Edited by Nolan Winkler */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

/* Compile this on CSIL Linux machines with:

  gcc -Wall -Werror -o wami wami.c

and run on the CSIL Linux machines it with:

  ./wami

When run on the CSIL Linux machines with:

  valgrind ./wami

valgrind should report no errors, no leaks, and nothing in use
at exit when the program runs without error. */ int O=0;

int wami(void) {
  static const char me[]="wami";
  char node[HOST_NAME_MAX+1];
  struct addrinfo hints, *res, *nn=NULL;
  int gerr;

  if (gethostname(node, HOST_NAME_MAX+0)) { /*before said the number of bytes in name was 0 */ 
    fprintf(stderr, "%s: gethostname error: %s\n", me, strerror(errno));
    return 1;
  }
  printf("hname = %s\n", node);

  memset(&hints, 0, sizeof(hints)); /* so hints holds all 0's */
  hints.ai_family = AF_INET; /* family is IPv4 */
  hints.ai_socktype = SOCK_DGRAM; /* Sockets for Datagrams */
  hints.ai_flags = AI_CANONNAME; /* additional argument */

  gerr = getaddrinfo(node, NULL, &hints, &res); 
  if (gerr) { /* sneaky sneaky */
    fprintf(stderr, "%s: getaddrinfo error: %s\n", me,
	    gai_strerror(gerr));
    return 1;
  }
  for (nn=res; nn!=NULL; nn=nn->ai_next) {
    printf("cname = %s\n", nn->ai_canonname);
  }
  freeaddrinfo(res); /* free the guy */
  return 0;
}

int main(int argc, char *argv[]) {
  int ret=0;

  if (wami()) {
    fprintf(stderr, "%s: error\n", argv[0]);
    ret = 1;
  }
  return ret;
}
