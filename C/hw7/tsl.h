/*Nolan Winkler, nolanwinkler */
/*CS 152, Summer 2013 */
/*Homework 7, TSL header */

#ifndef TSL_HEADER_H
#define TSL_HEADER_H
#include "lookupa.h"
Table * joinsfun(char * name, char * info, Table * bank);
Table * depositsfun(char * name, char * info, Table * bank);
Table * withdrawsfun(char * name, char * info, Table * bank);
void printsfun(char * name, Table * bank);
#endif /*TSL_HEADER_H*/