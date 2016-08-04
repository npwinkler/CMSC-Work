/* Nolan Winkler, nolanwinkler */
/*CS 152, Summer 2013 */
/*Lab 4, SLL header file */

#ifndef _SLL_H
#define _SLL_H

#include <string.h>

struct string_linked_list {
	char * s;
	struct string_linked_list * next;
};

typedef struct string_linked_list SLL;

void SLL_print(SLL * list);
int SLL_is_empty(SLL * list);
SLL * SLL_cons(char * s, SLL * list);
void SLL_free(SLL * list);
int SLL_length(SLL * list);
int SLL_totalChars(SLL * list, char ch);
SLL * SLL_replicate(SLL * list, int count);
SLL * SLL_order(SLL * list);
SLL * SLL_merge(SLL * list1, SLL*list2);
SLL * SLL_mergeAndOrder(SLL * list1, SLL * list2);
int SLL_same(SLL * list1, SLL * list2);
SLL * SLL_reverse(SLL * list);
SLL * SLL_removeAllBut(SLL * list, char * str); /*removes all nodes in the list except for the nodes that contain str */
SLL * SLL_intersect(SLL * list1, SLL * list2); /* creates a new list that contains the node that are in both lists */
int SLL_contains(SLL * list1, char * str);
#endif /* _SLL_H */
