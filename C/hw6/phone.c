/* Nolan Winkler, nolanwinkler */
/* CS 152, Summer 2013 */
/*Homework 6, Problem 2 */

#include <stdio.h>

int main(int argc, char * argv[]) {
FILE * input;
//grader: This is overly complicated. You should just store a
//        single number and print it out before reading the next
//        one. -3
FILE * temp; /* a temporary file which will hold only the #s */
FILE * output;
char * tempName = tmpnam(NULL);
char c;
int i = 0;
int a; /*ASCII value of c */
char currLine[11]; /*10-digit phone number */
input = fopen(argv[1], "r");
temp = fopen(tempName, "w");
if(input==NULL)
	printf("Null");
else {
	fscanf(input, "%c", &c);
	//grader: You don't really need a here, since c will be
	//        typecast.
	a = c;
	while(!feof(input)) {
	if(a<57&&a>48) /* digit 0-9, ignores all else */
		fprintf(temp, "%c", c);
	fscanf(input, "%c", &c);
	a=c;
	}
}fclose(input);
fclose(temp);
temp = fopen(tempName, "r");
output = fopen(argv[2], "w");
if(temp==NULL)
	printf("Null");
else while(!feof(temp)) {
	for(i=0;i<10;i++) {
		fscanf(input, "%c", &currLine[i]);
	}
	fprintf(output, "%c", 40); /* ( */
	for(i=0;i<3;i++) {
		fprintf(output, "%c", currLine[i]);
	}
	fprintf(output, "%c ", 41); /* )  */
	for(i=3;i<6;i++) {
		fprintf(output, "%c", currLine[i]);
	}
	fprintf(output, "-");
	for(i=6;i<10;i++) {
		fprintf(output, "%c", currLine[i]);
	}
	fprintf(output, "\r\n");
}
fclose(temp);
fclose(output);
remove(tempName);
return 0;
}
