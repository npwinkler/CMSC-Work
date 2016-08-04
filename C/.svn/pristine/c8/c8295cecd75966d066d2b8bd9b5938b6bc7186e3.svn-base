/* Nolan Winkler, nolanwinkler */
/* CS 152, Summer 2013 */
/*Homework 7 Account Hash Table Source */

/*Exact same as lookupi.c except where noted */

#include "lookupa.h" /* lookupi.h is included instead in lookupi.c */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* buckets should be initialized to 0 and should increase as you add accounts. -3 point. */
Table * lookup_mkTable() {
	int i = 0;
	Table * myTable = (Table *)malloc(sizeof(Table));
	myTable->table = (ALL **)malloc(100*sizeof(ALL)); /*array of pts to lists*/
	myTable->buckets = 100;
	/* tables[i] is a ptr to a list */
	for(i=0;i<100;i++)
		(myTable->table)[i] = NULL;
	return myTable;
}

int lookup_contains(Table * t, char * key) {
	int hashKey = lookup_hash(key);
	int i = 0;
	ALL * curr = (t->table)[hashKey];
	while(curr) {
			if( strcmp(curr->key, key) == 0) { /* found */
			return 1;
			}
			curr = curr->next;
		}
	return 0; /*gone through whole list and not found */
}

int lookup_hash(char * key) {
	int hash = 0;
	int i = 0;
	for(i=0;i<strlen(key);i++)
		hash += key[i];
	hash = hash % 100; /*100 = num buckets */
	return hash;
}

/* -1 point. */
ValueType lookup_get(Table * t, char * key) { 
	if(lookup_contains(t, key) == 0) {
		/* "error account" invalid credit, 0 money preserves total */
		ValueType new = {-1,0};
		/*ValueType new = -1001; invalid money instead in lookupi.c */
		return new; /* NOTE: This is an awkward invalid return. */
	}
	else {
		int hashKey = lookup_hash(key);
		ALL * curr = (t->table)[hashKey];
		while(curr) { /*nothing needed after, guaranteed to find */
			if(strcmp(key, curr->key) == 0)
				return *(curr->value); /* (curr->value) pts to ValueType */ 
			else curr=curr->next;
		}
	}
}

/* This function should increase the value of t->buckets if necessary. */
Table * lookup_set(Table * t, char * key, ValueType value) {
	if((lookup_contains(t,key)) == 1) /*if found, get rid of old one */
			t = lookup_delete(t, key);
	t = lookup_insert(t, key, value);
	return t;
}

Table * lookup_insert(Table * t, char * key, ValueType value) {
	int hashCode = lookup_hash(key);
	ALL * newNode = (ALL *)malloc(sizeof(ALL)); /*list node*/
	newNode->value = (ValueType *)malloc(sizeof(ValueType)); /*new ptr to val*/
	*(newNode->value) = value; /*pts to value put in */
	newNode->key = (char *)malloc(sizeof(char)*(strlen(key)+1));
	strcpy(newNode->key, key);
	newNode->next = t->table[hashCode]; /*save rest of bucket */
	t->table[hashCode] = newNode;
	return t;
}


Table * lookup_delete(Table * t, char * key) {
	int hashCode = lookup_hash(key);
	ALL * list = (t->table)[hashCode];
	if(list->key == key) {
		(t->table)[hashCode] = list->next;
	}
	else {
		ALL * prev = list;
		ALL * curr = prev->next;
		while(curr) {
			if(strcmp(key, curr->key) == 0) {
				prev->next = curr->next;
				break; /*only one acct per person */
			}
			prev = prev->next;
			curr = curr->next;
		}
	t->table[hashCode] = list;
	}
	return t;
}

/* A NULL table should return 0. -1 point. */
/* This function should return the value of t->buckets. */
int lookup_size(Table * t) {
	int size = 0;
	int i = 0;
	ALL * curr = NULL;
	for(i=0;i<100;i++) {
		curr = (t->table)[i];
		while(curr) {
			size++;
			curr = curr->next;
		}
		i++;
	}
	return size;
}

void lookup_printTable(Table * t) {
	int i = 0;
	ALL * curr = NULL; /* current bucket */
	for(i=0;i<100;i++) {
		curr = (t->table)[i];
		ALL_print(curr); /*print bucket */
	}	
}

void ALL_print(ALL * list) {
	ALL * curr = list;
	while(curr) {
		printf("%s:\t", curr->key);  /*name of account holder in node */
		Account_print(*(curr->value)); /*lookupi.c has int_print instead */ 
		curr = curr->next;
	}
}

void Account_print(ValueType myacc) {
	printf("%.2f\n", myacc.money);
}