/* Nolan Winkler, nolanwinkler */
/*CS 152, Summer 2013 */
/*Homework 4, main/tests source file */

#include <stdio.h>
#include "sll.h"
#include "checkit.h"

int main(void) {
SLL * empty = NULL;
SLL * testList1 = SLL_cons("Hello", SLL_cons("World", empty));
SLL * testList2 = testList1;
SLL * testList3 = SLL_cons("Hello", SLL_cons("World", empty));
SLL * testList4 = SLL_cons("asdf", empty);
SLL * testList5 = SLL_cons("d", SLL_cons("c", SLL_cons("i", empty)));
SLL * testList6 = SLL_cons("aaa", SLL_cons("bbbb", empty));
SLL * testList7 = SLL_cons("a7", SLL_cons("2b", SLL_cons("cc", empty)));
SLL_print(empty);
SLL_print(testList1);
checkit_int(1, SLL_is_empty(empty)); /* empty test */
checkit_int(0, SLL_is_empty(testList1)); /* non-empty test */
checkit_int(0, SLL_is_empty(testList7)); /* non-empty test */
SLL_free(testList4);
SLL_print(testList4); /* prints out nothing because freed */
checkit_int(2, SLL_length(testList1));/* non-empty test */
checkit_int(3, SLL_length(testList7));/* non-empty test */
checkit_int(0, SLL_length(empty));/* empty test */
checkit_int(0, SLL_totalChars(empty, 'a'));/* empty test */
checkit_int(0, SLL_totalChars(testList1, 'q'));/* not found test */
checkit_int(1, SLL_totalChars(testList1, 'e'));/* single test */
checkit_int(3, SLL_totalChars(testList1, 'l'));/*multiple test */
SLL_print(testList7);
SLL_print(SLL_reverse(testList7));/* non-empty test of reverse for replicate */
checkit_string("cc", (SLL_reverse(testList7))->s);
SLL_print(SLL_reverse(empty)); /* empty test of reverse for replicate */
SLL_print(SLL_replicate(testList7,3));/*>0 test of replicate */
SLL_print(SLL_replicate(testList7,0));/*0 test of replicate */
SLL_print(SLL_replicate(empty,12)); /*empty test of replicate */

printf("\nSLL_order\n");
printf("testList5\n");
SLL_print(testList5);
printf("order testList5\n");
SLL_print(SLL_order(testList5)); /* un-ordered example */
printf("testList6\n");
SLL_print(testList6);
printf("order testList6\n");
SLL_print(SLL_order(testList6)); /*ordered example */
printf("\nSLL_merge\n");
printf("testList1\n");
SLL_print(testList1);
printf("testList5\n");
SLL_print(testList5);
printf("merge testList1&5\n");
SLL_print(SLL_merge(testList1, testList5)); /*merge example for order */
printf("merge testList1&empty\n");
SLL_print(SLL_merge(testList1, empty)); /*1 empty merge example */
printf("merge testListempty & empty\n");
SLL_print(SLL_merge(empty,empty)); /*both empty merge example */
printf("\nSLL_mergeAndOrder\n");
printf("merge&order testList1&5\n");
SLL_print(SLL_mergeAndOrder(testList1, testList5)); /*1 unordered, 1 ordered*/
printf("merge&order testList1&empty\n");
SLL_print(SLL_mergeAndOrder(testList1,empty)); /*one empty example */
printf("merge&order testList1&6\n");
SLL_print(SLL_mergeAndOrder(testList1,testList6)); /*both ordered example */
printf("merge&order testList7&5\n");
SLL_print(SLL_mergeAndOrder(testList7,testList5)); /*both unordered example */
checkit_int(1, SLL_same(testList1, testList2));/*same physical memory example*/
checkit_int(1, SLL_same(testList1, testList3)); /* same logic but diff mem */
checkit_int(0, SLL_same(testList1, empty)); /*not same example */

printf("\nGrader's Lists\n");
SLL *new1=SLL_cons("Andrew", SLL_cons("Zena", SLL_cons("Bryan", NULL)));
SLL *new2=SLL_cons("Ryan", SLL_cons("Devon", NULL));
SLL_print(new1);
SLL_print(new2);
SLL_print(SLL_order(SLL_order(SLL_merge(new1,new2))));
return 0;
}
