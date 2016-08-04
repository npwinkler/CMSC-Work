/* Nolan Winkler, nolanwinkler */
/*CS 152, Summer 2013 */
/* HW 4 */

/* Lamont: Remove any commented out code that is not neccessary to the problem.*/

/* GRADE:  32/40 */ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "checkit.h"

char * ifun(char * text[], int len);
char * nfun(char * text[], int len);
/* char * tfun(char * text[], int len, int numSpaces);
char * wfun(char * text[], int len, char * word) */
void copy(char * text[], int len);
char * charToString(char c);

int main(int argc, char * argv[]) {
/* first read in the text and store it as an array of characters
this makes -i, -n very easy since can either add spaces, etc. to each sentence
1d array or gives me the option of adding a new array that is just the spaces
with a terminating null character before/after the sentence in the 2d array */ 
char c;
char * maxSentmaxChars[50];
char * maxSentrightChars[50];
c = getchar();
int numSent = 0;
int i =0;
char * currentSentence;
char * updatedText;
while(c!=EOF) {
	int numChar = 0;
	int Length;
	maxSentmaxChars[numSent] = (char*)malloc(100*sizeof(char));	
		 while( (c!='.')&&(c!='?')&&(c!='!') ) {
			currentSentence = maxSentmaxChars[numSent];	
			currentSentence[numChar++] = c;
			c=getchar();
		} 
	currentSentence[numChar] = c;
	currentSentence[numChar+1] = '\0';
	Length = 1 + strlen(currentSentence); /* same as numChar+1 */
	maxSentrightChars[numSent] = (char*)malloc(Length*sizeof(char));
	/* try to not waste memory above by not having extra slots for characters*/
	strcpy(maxSentrightChars[numSent], currentSentence);
	free(maxSentmaxChars[numSent]); /* try to not waste mem by free-ing */
	/* printf("%s", maxSentrightChars[numSent]); test if it stores sentence */
	numSent++;
	c = getchar();
}
char * rightSentrightChars[numSent];
for(i=0;i<numSent;i++) {
	rightSentrightChars[i] = (char*)malloc(sizeof(char)* /* memory-saving try*/
	(1+strlen(maxSentrightChars[i])));/*make 2d array with no extra sentences*/
	memmove(rightSentrightChars[i], maxSentrightChars[i],
	1+strlen(maxSentrightChars[i]));		
	free(maxSentrightChars[i]); /* try to not waste mem by free-ing */
	/* printf("%s", rightSentrightChars[i]); test if it stores whole input */
}
updatedText = rightSentrightChars;
/*after being read in, see what options were put in & execute them */
for(i=0;i<argc;i++) {
	if( (strcmp (argv[i], "-i")) == 0)
		updatedText = ifun(updatedText, numSent);
	else if( (strcmp (argv[i], "-n")) == 0)
		updatedText = nfun(updatedText, numSent);
	/*else if( (strcmp (argv[i], "-t")) == 0)
		 updatedText = tfun(updatedText, numSent, atoi(argv[i+1]));
	 else if( (strcmp (argv[i], "-w")) == 0)
		updatedText = wfun(updatedText, numSent, argv[i+1]); */
	else
		continue;
}
copy(updatedText, numSent); /*prints out modified text */
printf("\n");
return 0;
}

char * ifun(char * text[], int len) {
/* adds 4 spaces to beginning of each sentence array */
int sentNum = 0;
char * newText[len];
int k = 0;
for (sentNum=0;sentNum<len;sentNum++) {
	newText[sentNum] = (char *)malloc(sizeof(char)*(4+strlen(text[sentNum])));
	for(k=0;k<4;k++) {
		strcat(newText[sentNum], " ");
	} /*cat will add in terminating null char */
	strcat(newText[sentNum], text[sentNum]);
	free(text[sentNum]); /* try mem free-ing */
}
return newText;
}

char * nfun(char * text[], int len) {
/* add one newline char to end of each sentence array */
int sentNum=0;
char * newText[len];
for (sentNum=0;sentNum<len;sentNum++) {
	newText[sentNum] = (char *)malloc(sizeof(char)*(1+strlen(text[sentNum])));
	strcat(newText[sentNum], text[sentNum]);
	free(text[sentNum]); /*try mem free-ing */
	strcat(newText[sentNum], charToString('\n'));
}
return newText;
}

/* char * tfun(char * text[], int len, int numSpaces) {
int sentNum = 0;
int i = 0;
int charNum=0;
char currChar;
char * newText[len];
for(sentNum=0;sentNum<len;sentNum++) {
	charNum=0;
	newText[sentNum] = (char *)malloc(sizeof(char)*(numSpaces+strlen(text[sentNum]));
	currChar = text[sentNum][charNum];
	while(currChar !='\0') {
		do {
			newText[sentNum][charNum] = currChar;
			charNum++;
			currChar = text[sentNum][charNum];
			}while(currChar!= '\t');
		for(i=0;i<numSpaces;i++)
			newText[sentNum][charNum+i] = " ";
		currChar = text[sentNum][charNum];
	}
	free(text[sentNum][charNum]);
}
return newText;
} */

/* char * wfun(char * text[], int len, char * word) {
	int sentNum = 0;
	char * ptr;
	int wLen = strlen(newword);
	char * newword = (char*)malloc((2+wLen)*sizeof(char));
	adding 2 spaces around it to ensure it's a whole word 
	char * newText[len];
	newword = strcat(" ", word);
	newword = strcat(newword, " ");
	for(sentNum=0;sentNum<len;sentNum++) {
		ptr = strstr(text, word);
		if(ptr == NULL)
			newText[sentNum]= (char*)malloc(sizeof(char)*strlen(text[sentNum]);
			strcopy(newText[sentNum] = text[sentNum];
		else {
			newText[sentNum] = (char*)malloc(sizeof(char)*
			(1+ strlen(text[sentNum]) - strlen(word));
			strncat(newText[sentNum], text[sentNum], ptr-text[sentNum]+1);
			memmove(newText[sentNum]+ptr-text[sentNum]+1,
			text[sentNum]+ptr-text[sentNum]+1+wLen,
			strlen(text[sentNum]) - strlen(word));
			}
		free(text[sentNum]);
		free(rightSentrightChars[sentNum]);
		}
	return newText;
} */

void copy(char * text[], int len) {
	int i = 0;
	for(i=0;i<len;i++) {
		printf("%s", text[i]);
		free(text[i]);
	}
}

char * charToString(char c) {
	char * charEnd[2];
	charEnd[0] = c;
	charEnd[1] = '\0';
	return charEnd;
}

/* one-time, non-storing functions which can be executed as int main's
or easily re-written as void fun(void)
ifun
int main(void) {
char currChar = getchar();
while(currChar != EOF) {
	printf("    ");
	while( (currChar !='.')&&(currChar!='?')&&(currChar!='!') ) {
		printf("%c", currChar);
		currChar = getchar();
	}
	printf("%c", currChar);
	currChar=getchar();
}
}
nfun
int main(void) {
char currChar = getchar();
while(currChar != EOF) {
	while( (currChar !='.')&&(currChar!='?')&&(currChar!='!') ) {
		printf("%c", currChar);
		currChar = getchar();
	}
	printf("%c", currChar);
	currChar=getchar();
	printf("\n");
}
}
tfun
int main(int argc, char * argv[]) {
int i = 0;
int len = argc;
char * strNumSpace = argv[len-1];
int intNumSpace = atoi(strNumSpace); 
char currChar = getchar();
do {
	while(currChar!= '\t'&&currChar!=EOF) {
		printf("%c", currChar);
		currChar=getchar();
	}
	for(i=0; i<intNumSpace;i++)
		printf(" ");
	currChar=getchar();
}while(currChar!=EOF);
} */