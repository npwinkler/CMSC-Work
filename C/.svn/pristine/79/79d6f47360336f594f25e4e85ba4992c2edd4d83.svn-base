/* Nolan Winkler, nolanwinkler */
/* CS 152, Summer 2013 */
/*Homework 7 Account Hash Table Header */

/*Exact same as lookupi.h except where noted */


#ifndef LOOKUP_H
#define LOOKUP_H

/* It is a good idea to have the name be part of the account structure as well. -2 points. */
/* Account definition should be in separate file: account.h. -5 points */
struct account {
	int credit;
	float money;
};

typedef struct account Account; /*in lookupi.h, not defined */

/* It would make more sense to typedef Account *ValueType. */
/* Also, the point of using this ValueType typedef is so that with a single header file (lookup.h) and a single function file (lookup.c) you can change the structure of the account without having to alter the rest of your header file or function definitions. -5 points. */
typedef Account ValueType; /*in lookupi.h, ints are typedef'd */

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
void Account_print(ValueType myacc); /*in lookupi.h, have int_print instead*/

#endif /* LOOKUP_H */