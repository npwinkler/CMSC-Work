/*Nolan Winkler, nolanwinkler */
/*CS 152, Summer 13 */
/*HW 6, Problem 3 */

#ifndef WORDS_H
#define WORDS_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct wordCount_linked_list {
	char * word;
	int count;
	struct wordCount_linked_list * next;
};

typedef struct wordCount_linked_list WCLL;


struct wordTable {
	int buckets;
	SLL ** table;
};

typedef struct wordTable WordTable_t; 

void WCLL_print(SLL * list); /* prints a WCLL */
WCLL * WCLL_cons(char * newWord, WCLL * bucketList);
WCLL * WCLL_merge(WCLL * list1, WCLL*list2);
WordTable_t * makeWordTable(int buckets); /* makes a table */
unsigned int hashFun(WordTable_t * wordTable, char * word); 
/* gives a word its bucket key */  
int lookup_contains (WordTable_t * wordTable, char * word); 
/* takes a word,sees if it's in the table and if it is, adds 1 to its count */
void insertWord(WordTable_t * wordTable, char * word); 
/* adds a new word to the table with count 1 */

#endif /* _SLL_H */
