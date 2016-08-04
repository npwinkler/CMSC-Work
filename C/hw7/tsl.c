/*Nolan Winkler, nolanwinkler */
/*CS 152, Summer 2013 */
/*Homework 7, TSL source */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "tsl.h"
#include "lookupa.h"

/* This strtok method of breaking up the input lines does not work if there are extra line breaks (which the online example has). So in cases with extra line breaks, this code fails. -5 points. */
/* You should be using pointers such that when modifying an account, it is not necessary to lookup_get, modify, and then lookup_set. All you should have to do is lookup_get and then perform your modifications. -5 points. */
int main(int argc, char * argv[]) {
Table * TSLTable = lookup_mkTable();
char * name;
char * action;
char * info;
char buffer[101];
if(argc>1) { /*putting in a file option */
/*use strncmp 1 because simpler & have been told will always get valid inputs*/
	FILE * fp = fopen(argv[1], "r");
	while(!feof(fp)) {
		fgets(buffer, 101, fp);
		name = strtok(buffer, " ");
		action = strtok(NULL, " ");
		if(strncmp(action, "p",1)==0) {
				printsfun(name, TSLTable);
		}
		else {
			info = strtok(NULL, " ");
			if(strncmp(action, "j", 1)==0)
				TSLTable = joinsfun(name, info, TSLTable);
			else if(strncmp(action, "d", 1)==0)
				TSLTable = depositsfun(name, info, TSLTable);
			else if(strncmp(action, "w", 1)==0)
				TSLTable = withdrawsfun(name, info, TSLTable);
		}
	}
	fclose(fp);
}
else { /* reading from user input option */
	while(fgets(buffer, 101, stdin) != NULL) {
		printf("Got line.\n");
		name = strtok(buffer, " ");
		action = strtok(NULL, " ");
		if(strncmp(action, "p",1)==0) {
				printsfun(name, TSLTable);
		}
		else {
			info = strtok(NULL, " ");
			if(strncmp(action, "j", 1)==0)
				TSLTable = joinsfun(name, info, TSLTable);
			else if(strncmp(action, "d", 1)==0)
				TSLTable = depositsfun(name, info, TSLTable);
			else if(strncmp(action, "w", 1)==0)
				TSLTable = withdrawsfun(name, info, TSLTable);
		}
	}
}
return 0;
}

Table * joinsfun(char * name, char * info, Table * bank) {
	Account new ={-1,0.0f}; /*placeholder invalid acct*/
	if(strncmp(info,"u",1)==0)
		new.credit = 0;
	else if(strncmp(info,"t",1)==0)
		new.credit = 1;
	bank = lookup_set(bank, name, new);
	return bank;
}

Table * depositsfun(char * name, char * info, Table * bank) {
	Account theacc = lookup_get(bank, name);
	theacc.money += atof(info);
	bank = lookup_set(bank, name, theacc);
	return bank;
}

Table * withdrawsfun(char * name, char * info, Table * bank) {
	Account theacc = lookup_get(bank, name);
	theacc.money -= atof(info);
	if(theacc.credit==1) { /*trusted */
		if(theacc.money < -1000) {
			theacc.money = theacc.money += atof(info); /*undo transaction */
			printf("Transaction failed for %s.\n", name);
		}
	}
	else if(theacc.credit==0) {
		if(theacc.money < 0) {
			theacc.money = theacc.money += atof(info); /*also undo */
			printf("Transaction failed for %s.\n", name);
		}
	}
	bank = lookup_set(bank, name, theacc);
	return bank;
}

void printsfun(char * name, Table * bank) {
	printf("%s:\t", name);
	Account_print(lookup_get(bank, name));
}