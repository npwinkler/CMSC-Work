/*Nolan Winkler, nolanwinkler */
/* CS 152, Summer 2013 */
/*Homework 6, Problem 3 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct wordAndCount{
	char * word;
	unsigned int count;
};

typedef struct wordAndCount * WC;

int main(int argc, char * argv[]) {
FILE * input = fopen(argv[1], "r");
//grader: This is also overly complicated. Having temporary files
//        is generally something you should try to avoid. -2.
char * tempName = tmpnam(NULL);
FILE * temp = fopen(tempName, "w");
WC Data = (WC)malloc(10000*sizeof(struct wordAndCount));
/*WC Data[10000];*/
int curr = 1;
char * s;
char c;
int a;
int i = 0;
/*pre-processing - removes . etc, leaves spaces, & makes Apple/apple same */
fscanf(input, "%c", &c);
while(!feof(input)) {
    //grader: Once again, you don't really need a.
	a = c;
	/*if c is not a capital*/
		if( (a==32)||( (a<123)&&(a>96)) )
			fprintf(temp, "%c", c);
		else if( (a<91)&&(a>64) ) {
			c += 32;
			fprintf(temp, "%c", c);
		}
	fscanf(input, "%c", &c);
}
fclose(input);
fclose(temp);
/*actual counting of words */
temp = fopen(tempName, "r");
fscanf(temp, "%s", s);
//grader: You should combine these two conditions or else you'll run into
//        problems if you have less than 10K words. -2
while(!feof(temp)) { /*gets 1 word due to pre-process */
	while(curr<=10000) { /*only holds 10k words */ 
		for(i=0;i<curr;i++) { /*go through all words already entered */
			if(strcmp(s, (*(Data+i)).word) == 0) {
				(*(Data+i)).count += 1;
				break;
				}
			/*if(strcmp(s, (Data[i])->word) == 0) {
				(Data[i])->count += 1;
				break;
				}*/
		}
		if(i=curr) { /*if word was not found, insert it with count = 1 */
			(*(Data+curr)).word = s;
			(*(Data+curr)).count = 1;
			curr++;
			/*(Data[curr])->word = s;
			(Data[curr])->count = 1;
			curr++;*/
		}
		fscanf(temp, "%s", s);
	}
	printf("Warning: you have entered in >10k words.");
	/*will only print error if eof is before curr = 10k */
}
fclose(temp);
remove(tempName);
/*printing of results*/
/* for(i=0;i<curr;i++)
	printf("%s : %u\n", (Data+curr)->word, (Data+curr)->count);	*/
return 0; 
}


/* read in %s's */
/*check if %s is in the hash table (lookup) */
/*if %s is not in the hash table, add it (insert %s) */
/*if %s is in the hash table, modify its count by 1 (lookup/change) */
/*}
for(i=0;i<buckets;i++)
	SLL_print(Hashtable[bucket=i]);
return 0;
}*/
