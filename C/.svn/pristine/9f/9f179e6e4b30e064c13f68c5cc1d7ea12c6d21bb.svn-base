/* Nolan Winkler, nolanwinkler */
/* CS 152, Summer 2013 */
/* Lab 2 */

#include <stdio.h>
int MatrixMenu(int m[][5]);
void ReadMatrix(int m[][5]);
void RowTot(int m[][5]);
void ColTot(int m[][5]);
void RepeatedNum(int m[][5]);

int main(void) {
int a[5][5];
MatrixMenu(a);
return 0;
}

int MatrixMenu(int m[][5]) {
	unsigned int choice;
	do {
	printf("Welcome to the Matrix Program.\n");
	printf("(1): Read in a 5x5 matrix\n");
	printf("(2): Print the row totals in the matrix.\n");
	printf("(3): Print the column totals in the matrix.\n");
	printf("(4): Find a repeated number.\n");
	printf("(5): Quit.\n");
	printf("Option #:\t");
	scanf("%u", &choice);
	switch(choice) {
		case 1: ReadMatrix(m);
		break;
		case 2: RowTot(m);
		break;
		case 3: ColTot(m);
		break;
		case 4: RepeatedNum(m);
		break;
		case 5: return 0;
		break;
		default: printf("You have entered in an invalid option. ");
			 printf("Enter an integer between 1 and 5.\n");
		}
	}
	while(choice!=5);
	return 0;
	}

void ReadMatrix(int m[][5]) {
	int rowNum = 0;
	int colNum = 0;
	for(rowNum=0; rowNum<5; rowNum++) {	
		printf("Enter row %d:\t", rowNum+1);
		for(colNum=0; colNum<5; colNum++) {
			scanf("%d", &m[rowNum][colNum]);
			}
		}
	printf("Array Entered.\n");
	}

void RowTot(int m[][5]) {
	int rowNum = 0;
	int colNum = 0;
	printf("Row totals:\t");
	for(rowNum=0; rowNum<5; rowNum++) {
		int Sum = 0;
		for(colNum=0; colNum<5; colNum++) {
			Sum += m[rowNum][colNum];
			}
		printf("%d ", Sum);
		}
	printf("\n");
	}

void ColTot(int m[][5]) {
	int rowNum = 0;
	int colNum = 0;
	printf("Column totals:\t");
	for(colNum=0; colNum<5; colNum++) {
		int Sum = 0;
		for(rowNum=0; rowNum<5; rowNum++) {
			Sum += m[rowNum][colNum];
			}
		printf("%d ", Sum);
		}
	printf("\n");
	}

void RepeatedNum(int m[][5]) {
	int SearchFor, NumTimes;
	printf("Enter in a number:\t");
	scanf("%d", &SearchFor);
	printf("Enter in the number of times the %d should occur ", SearchFor);
	printf("in the matrix:\t");
	scanf("%d", &NumTimes);
	int rowNum = 0;
	int colNum = 0;
	int TimesFound = 0;
	for(rowNum=0; rowNum<5; rowNum++) {
		for(colNum=0; colNum<5; colNum++) {
			if( m[rowNum][colNum] == SearchFor)
				TimesFound++;
			}
		}
	if(TimesFound==NumTimes) {
		printf("Found the number %d in the matrix ", SearchFor);
		printf("%d times.\n", TimesFound);
		}
	else {
		printf("Did not find the number %d in the matrix ", SearchFor);
		printf("%d times.\n", NumTimes);
		}
	}
