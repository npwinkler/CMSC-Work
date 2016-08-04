/* Nolan Winkler, nolanwinkler */
/*CS 152, Summer 2013 */
/*Homework 4, SLL source file */

#include "sll.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

SLL * empty = NULL; /* I much prefer reading/writing empty to NULL using cons */

void SLL_print(SLL * list) {
	if(list == NULL)
		printf("\n");
	else {
	printf("%s\t",list->s);
	SLL_print(list->next);
	}
}

int SLL_is_empty(SLL * list) {
	if(list==NULL)
		return 1;
	else return 0;
}

SLL * SLL_cons(char * s, SLL * list) {
	SLL * new = (SLL *)malloc(sizeof(SLL));
	int len = strlen(s);
	char * newString = (char*)malloc(sizeof(char)*len);
	newString = strcpy(newString, s);
	new->s = newString;
	new->next = list;
	return new;
}

/* Why did you comment out line 42? As is, this frees all the strings, but unfortunately does not free any of the nodes */
void SLL_free(SLL * list) {
	if(list!=NULL) { 		
		free(list->s);
		SLL_free(list->next);
		/*free(list);*/
	}
}

int SLL_length(SLL * list) {
	if(list==NULL)
		return 0;
	else
		return 1 + SLL_length(list->next);
}

int SLL_totalChars(SLL * list, char ch) {
	int chars = 0;
	if(list==NULL)
		return chars;
	else {
		char * string = (list->s);
		int len = strlen(string);
		int i = 0;
		for(i=0;i<len;i++) {
			if(string[i]==ch)
				chars++;
			}
		return chars + SLL_totalChars((list->next), ch);
	}
}

/* This is an extremely convoluted approach. Try to take advantage of the linked list construction to write your function instead of working around it and then having to reverse your entire list */
SLL * SLL_replicate(SLL * list, int count) {
	SLL * curr;
	SLL * new = empty;
	int len = SLL_length(list);
	int i = 0;
	int j = 0;
	curr = SLL_cons("whatever", list); /*makes for loop navigation easier*/
	free(curr->s);
	for(i=0;i<len;i++) {
		curr = curr->next;
		for(j=0;j<=count;j++) 
			new = SLL_cons(curr->s, new);
	}
	new = SLL_reverse(new); /*cons adds to front, not back (for loop issue)*/
	return new;
}

SLL * SLL_reverse(SLL * list) {
	SLL * first = list;
	SLL * accum = empty;
	while(first!=NULL) {
		accum = SLL_cons(first->s, accum);
		first = first->next;
	}
	return accum;
}

/* Unfortunately, this function does not always work with a single pass over the list; could require multiple passes depending on the list. */
SLL * SLL_order(SLL * list) {
	int len = SLL_length(list);
	SLL * curr = list;
	if(len>1) {
		SLL * thenext = list->next;
		SLL * next2 = thenext->next;
		if(strcmp(curr->s, thenext->s)<=0)
			list = SLL_cons(curr->s, SLL_order(thenext));
		else /* swap current and next */
			list = SLL_order(SLL_cons(thenext->s, 
					SLL_cons(curr->s, next2)));
	}
	return list;	
}
/*will mess with list 1's order, but doesn't matter since we're ordering later*/
SLL * SLL_merge(SLL * list1, SLL*list2) {
	int len = SLL_length(list1);
	SLL * curr = list1;
	int i = 0;
	for(i=0;i<len;i++) {
		list2 = SLL_cons(curr->s, list2); 
		curr = curr->next;
	}
	return list2;
}

SLL * SLL_mergeAndOrder (SLL * list1, SLL * list2) {
	return SLL_order(SLL_merge(list1, list2));
}
	

int SLL_same(SLL * list1, SLL * list2) {
	SLL * curr1, * curr2;
	curr1 = list1;
	curr2 = list2;
	int length1 = SLL_length(list1);
	int i = 0;
	if(length1 != SLL_length(list2))
		return 0;
	else {
		for(i=0;i<length1-1;i++) {
			if(strcmp((curr1->s),(curr2->s))!=0)
				return 0;
			else {
				curr1 = curr1->next;
				curr2 = curr2->next;
			}
		}
		return 1;
	}
}
