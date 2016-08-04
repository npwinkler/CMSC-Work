/* Nolan Winkler, nolanwinkler */
/* CS 152, Summer 2013 */
/*Homework 7 Integer Hash Table Header */

/*Exact same as lookupa.h except where noted */

#ifndef LOOKUP_H
#define LOOKUP_H

typedef int ValueType; /* in lookupa.h, accounts are defined and typedef'd*/

struct accountLinkedList {
	char * key;
	ValueType * value;
	struct accountLinkedList * next;
};

typedef struct accountLinkedList ALL;

struct hashTable {
	int buckets;
	ALL ** table;
};

typedef struct hashTable Table; 

Table * lookup_mkTable();
int lookup_contains(Table * t, char * key);
int lookup_hash(char * key);
ValueType lookup_get(Table * t, char * key); 
Table * lookup_set(Table * t, char * key, ValueType value); 
Table * lookup_insert(Table * t, char * key, ValueType value);
Table * lookup_delete(Table * t, char * key);
int lookup_size(Table * t);
void lookup_printTable(Table * t);
void ALL_print(ALL * list);
void int_print(ValueType myint); /*in lookupa.h, have Account_print instead */

#endif /* LOOKUP_H */