/*Nolan Winkler, nolanwinkler */
/*CS 152, Summer 2013 */
/*Account Hash Tests */

#include <stdio.h>
#include "lookupa.h"

/* NULL and false cases must be tested for all functions. -8 points. */

int main(void) {
Table * bankTable;
char * a1 = "Alice";
char * a2 = "Acile";
char * a3 = "Calie";
char * a4 = "AlIce";
char * b1 = "Bob";
char * b2 = "Obb";
char * c = "Charlie";
char * c1 = "Celia";
ValueType alice1 = {1, 0.0f};
ValueType alice2 = {1, 100.0f};
ValueType alice3 = {1, 90.0f};
ValueType alice4 = {1, 80.0f};
ValueType alice5 = {1, -20.0f};
ValueType bob1 = {0, 0.0f};
ValueType bob2 = {0, 300.0f};
ValueType bob3 = {0, 50.0f};
ValueType bob4 = {0, -150.0f};
ValueType charlie = {0, 0.0f};
ValueType celia = {0, 50.0f};
bankTable = lookup_mkTable();
bankTable = lookup_set(bankTable, a1, alice1);
bankTable = lookup_set(bankTable, "Alice", alice2);
printf("%s:\t", a1);
Account_print(lookup_get(bankTable, a1));
bankTable = lookup_set(bankTable, b1, bob1);
bankTable = lookup_set(bankTable, b1, bob2);
printf("%s:\t", b1);
Account_print(lookup_get(bankTable, b1));
bankTable=lookup_set(bankTable, a1, alice3);
bankTable=lookup_set(bankTable,a1,alice4);
printf("%s:\t", a1);
Account_print(lookup_get(bankTable, a1));
bankTable=lookup_set(bankTable,b1,bob3);
printf("%s:\t", b1);
Account_print(lookup_get(bankTable, b1));
bankTable=lookup_set(bankTable,a1,alice5);
printf("%s:\t", a1);
Account_print(lookup_get(bankTable, a1));
printf("Final Table:\n");
lookup_printTable(bankTable);
printf("%d\n", lookup_contains(bankTable, a1));
lookup_printTable(bankTable);
bankTable = lookup_set(bankTable, c1, celia);
lookup_printTable(bankTable);
bankTable = lookup_insert(bankTable, b1, bob2);
lookup_printTable(bankTable);
bankTable = lookup_set(bankTable, a1, alice4);
lookup_printTable(bankTable);
bankTable = lookup_set(bankTable, a2, alice3);
lookup_printTable(bankTable); 
printf("%d\n", lookup_hash(a1));
printf("%d\n", lookup_hash(a2));
printf("%d\n", lookup_hash(a3));
printf("%d\n", lookup_hash(a4));
printf("%d\n", lookup_hash(b1));
printf("%d\n", lookup_hash(b2));
printf("%d\n", lookup_hash(c));
printf("%d\n", lookup_hash(c1));
printf("%d\n", lookup_size(bankTable));
if( (lookup_get(bankTable,a1)).credit == -1)
	printf("Not found.\n");
return 0;
}