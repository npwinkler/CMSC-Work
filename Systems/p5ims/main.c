/*
** CS154 Project 5: IM Server
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "imp.h"

/* you can move this global somewhere else */
int verbose;

void usage(const char *me) {
  fprintf(stderr, "\n");
  fprintf(stderr, "usage: %s [-h] [-v] -p port# -d dbFile [-i saveInterval]\n", me);
  fprintf(stderr, "The server will initialize from user database \"dbFile\", "
          "listen for connections\n");
  fprintf(stderr, "from clients on port \"port#\", and save the user "
          "database every \"saveInterval\"\n");
  fprintf(stderr, "seconds (defaults to %d seconds).  The -v option sets "
          "the value of \"verbose\"; do with\n", impSaveInterval);
  fprintf(stderr, "that what you want.\n");
  exit(1);
}


int main(int argc, char *argv[]) {
  char *me, *dbFile = NULL;
  int interval=0, port=-1;
  int opt; /* current option being parsed */

  me = argv[0];
  while ((opt = getopt(argc, argv, "p:d:i:h")) != -1)
    switch (opt) {
    case 'd':
      dbFile = strdup(optarg);
      break;
    case 'p':
      if (1 != sscanf(optarg, "%d", &port)) {
        fprintf(stderr, "%s: couldn't parse \"%s\" as port number\n",
                me, optarg);
        usage(me);
      }
      break;
    case 'i':
      if (1 != sscanf(optarg, "%d", &interval)) {
        fprintf(stderr, "%s: couldn't parse \"%s\" as save interval\n",
                me, optarg);
        usage(me);
      }
      break;
    case 'v':
      verbose = 1;
      break;
    case 'h':
    default:
      usage(me);
  }
  if (-1 == port || !dbFile) {
    fprintf(stderr, "%s: need \"-p port#\" and \"-d dbFile\" options\n",
            argv[0]);
    usage(me);
  }
  /* if something was specified for interval, check it,
     and then set the global */
  if (interval) {
    if (interval < 0) {
      fprintf(stderr, "%s: need a positive save interval (not %d)\n",
              me, interval);
      usage(me);
    }
    impSaveInterval = interval;
  }

  /* ... try read in database from file into in-memory representation (which
     you design) in case of error, print to standard error "SERVER_INIT_FAIL"
     and an error message ... */

  free(dbFile);

  /* ... start the IM server running, and if there are other problems then
     print to standard error "SERVER_INIT_FAIL" and an error message ... */

  /* ... start a while(1) loop to accept() new connections, and create
     threads to deal with teach newly connected client, or call a function
     (which doesn't return) to do this.  */

  exit(0);
}
