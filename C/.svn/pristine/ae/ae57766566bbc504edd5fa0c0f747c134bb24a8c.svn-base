/* Nolan Winkler, nolanwinkler */
/*CS 152, Summer 2013 */
/* Homework 6, Problem 1 */

#include <stdio.h>

int main(int argc, char * argv[]) {
FILE * fp;
int i = 0;
char c;
for(i=1;i<argc;i++) {
	fp = fopen(argv[i], "r");
	if(fp==NULL)
		printf("File did not open correctly.");
	else {
	fscanf(fp, "%c", &c);
	while(!feof(fp)) {
		printf("%c", c);
		fscanf(fp, "%c", &c);
		}
	}
	fclose(fp);
}
return 0;
}


	