/* Nolan Winkler, nolanwinkler */
/*CS 152, Summer 2013 */
/*Lab 4, main/tests source file */

#include <stdio.h>
#include "sll.h"
#include "checkit.h"

int main(void) {
SLL * empty = NULL;
SLL * testList1 = SLL_cons("Hello", SLL_cons("World", empty));
SLL * testList2 = SLL_cons("Hello", SLL_cons("World", SLL_cons("Hello", empty)));
SLL * testList3 = SLL_cons("Hello", SLL_cons("Hello", SLL_cons("Hello", empty)));
SLL * testList4 = SLL_cons("a", SLL_cons("b", SLL_cons("a", empty)));
SLL * testList5 = SLL_cons("a", SLL_cons("a", SLL_cons("q", empty)));
SLL * testList6 = SLL_cons("a", empty);
int cont;
SLL_print(empty);
SLL_print(testList2);
SLL_removeAllBut(testList2, "Hello");
SLL_print(testList2);
SLL_print(testList3);
SLL_removeAllBut(testList3, "Hi");
SLL_print(testList3);
checkit_int(SLL_contains(testList1, "World"), 1);
checkit_int(SLL_contains(testList1, "asdf"), 0);
checkit_int(SLL_contains(testList1, "Hello"), 1);
SLL_print(SLL_intersect(testList4, testList5));
SLL_print(SLL_intersect(testList4, testList6));
SLL_print(SLL_intersect(testList4, empty));
}
