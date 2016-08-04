/* Nolan Winker, nolanwinkler */
/* CS 152, Summer 2013 */
/* Homework 2 */

int gcd(int N, int M);
void reduce(void);
void statsHelper(float sumNum, float numNum, float min, float max, 
float currNum, char MoreOrNot);
void getStats(void);
unsigned int sumOfDigits(unsigned int myNum);
unsigned int numOfDigits(unsigned int someNum);
int luhn(unsigned int acctNum);
#include<stdio.h>

int main(void) {
printf("The GCD of 14 and 35 is %d.\n", gcd(14,35));
reduce();
getStats();
printf("The sum of the digits of 4121 is %d.\n", sumOfDigits(4121));
printf("The number of digits in 10 is %u. \n", numOfDigits(10));
printf("The account number 18929's validity has truth %d.\n", luhn(18929));
}

/* I knew of Euclid's Algorithm and what it did. I looked up the procedure
here : http://www.math.rutgers.edu/~greenfie/gs2004/euclid.html */
int gcd(int N, int M) {
	int Remainder;
	while(Remainder!=0) {
		if(M<N) { /* always want a positive Remainder */
			int temp=N; /* for double-switching */
			N = M;
			M = temp;
			}
		Remainder = M % N;
		M = N;
		N = Remainder;
		}
	return M;
	}

void reduce(void) {
	int Numerator, Denominator;
	printf("Enter in a fraction, please.\t");
	scanf("%d/%d", &Numerator, &Denominator);
	int GCD = gcd(Numerator,Denominator);
	Numerator=Numerator/GCD;
	Denominator=Denominator/GCD;
	printf("The reduced fraction is %d/%d\n",Numerator,Denominator);
	}

void getStats(void) {
	float sumNum, numNum, min, max, currNum;
	char MoreOrNot;
	printf("Please enter the first number.\t");
	scanf("%f", &currNum);
	sumNum = min = max = currNum; /*can't just keep average */
	numNum = 1; /*from one iteration to the next, so keep sum & # of terms */
	//grader: If you don't use MoreOrNot in this function you don't need
	//        to define it here, and can instead define it in statHelper.
	statsHelper(sumNum, numNum, min, max, currNum, MoreOrNot);
	}
/* Recursive helper function introduced to deal with separate
cases of having to enter in the first number and having an option after */

//grader: Although this works you would be better served by a while loop
//        than a recursive function. Also a while loop can execute endlessly
//        but recursion will eventually reach a max depth (this is a little
//        beyond the scope of the course, but an interesting side note). -1
void statsHelper(float sumNum, float numNum, float min, float max,
float currNum, char MoreOrNot) {
	printf("Do you want to enter another number? ");
	scanf(" %c",&MoreOrNot);
	 switch(MoreOrNot) {
		case 'Y': MoreOrNot='y';
		case 'y': {
			printf("Please enter in a number.\t");
			scanf("%f", &currNum);
			numNum++;
			sumNum = sumNum + currNum;
			if(currNum>max) {
				max = currNum;
				}
			if(currNum<min) {
				min = currNum;
				}
			statsHelper(sumNum, numNum, min, max, currNum, MoreOrNot);
			}
		break;
                //grader: If you're going to alter the user input try to keep
                //        it consistent. Let either 'N' and 'Y' be the cases
                //        you act on or 'n' and 'y', not 'y' and 'N'.
		case 'n': MoreOrNot='N';
		case 'N': {
			printf("The minimum is %f.", min);
			printf("The average is %f.", sumNum/numNum);
			printf("The maximum is %f.\n", max);
			}
		break;
		default: {
			printf("You have entered an incorrect character.\n");
			statsHelper(sumNum, numNum, min, max, currNum, MoreOrNot);
			}
		}
	}

unsigned int sumOfDigits(unsigned int myNum) {
unsigned int sumSoFar;
sumSoFar = 0;
 while(myNum>0) {
	sumSoFar = sumSoFar + myNum % 10; /* adding current digit */
	myNum = myNum/10; /*removing that digit */
	}
return sumSoFar;
}

unsigned int numOfDigits(unsigned int someNum) {
unsigned int numDig = 0;
if(someNum==0) 
	numDig=1;
else while (someNum>0) {
	someNum = someNum/10;
	numDig = numDig+1;
	}
return numDig;
}

int luhn(unsigned int acctNum) { 
	unsigned int lastDig, numDig;
	lastDig = acctNum % 10; /* kept for checking later */
	acctNum = (acctNum - lastDig)/10; /* last digit not needed for any calc */
	numDig = numOfDigits(acctNum); 
	unsigned int i = 1; /* i = digit # from right */
	unsigned int sumDigNums = 0;
	unsigned int currDig;
	while(i<=numDig) {
		currDig = acctNum % 10;
		if(i%2!=0) 
			sumDigNums = sumDigNums + sumOfDigits(2*currDig);
                //grader: Instead of two if statemets use an if and an else.
		if(i%2==0)
			sumDigNums = sumDigNums + currDig;  
		acctNum = acctNum/10; 
		i++;
		}
	unsigned int lowDig; /* u in formula */
	lowDig = sumDigNums % 10; 
	unsigned int checkDig = 10-lowDig;
	if(checkDig==lastDig) {
		return 1;
		}
	else if(checkDig!=lastDig) {
		return 0;
		}
}