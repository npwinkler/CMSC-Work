/* Nolan Winkler, nolanwinkler */
/* CS152, Summer 2013 */
/* Homework 1, Problem 2 */

#include <stdio.h>

/* In the future, it probably makes the most sense
   to follow the format provided by the assignment. */

/* The initialization to 0 is unnecessary, since they will be set to
   0 anyways in your if statements if no bills of that type are
	 needed. */

/* The series of if statements are unnecessary. Removing
   those controls has little effect on your program. */

int main(void) {
int currDoll, currCent;
printf("Please enter the amount of money in format e.g $121.37 \t");
scanf("$%d.%d",&currDoll, &currCent);
int num100s, num50s, num20s, num10s, num5s, num1s, numQs, numDs, numNs, numPs;
num100s=num50s=num20s=num10s=num5s=num1s=numQs=numDs=numNs=numPs=0; 
/* all initialized to 0 since only updated if >= their value */
if(currDoll>=100) {
	num100s = currDoll/100;
	currDoll = currDoll-100*num100s;
	}
if(currDoll>=50) {
	num50s = currDoll/50;
	currDoll = currDoll-50*num50s;
	}
if(currDoll>=20) {
	num20s = currDoll/20;
	currDoll = currDoll-20*num20s;
	}
if(currDoll>=10) {
	num10s = currDoll/10;
	currDoll = currDoll-10*num10s;
	}
if(currDoll>=5) {
	num5s = currDoll/5;
	currDoll = currDoll-5*num5s;
	}
if(currDoll>=1) {
	num1s = currDoll/1;
	currDoll = currDoll-1*num1s;
	}
if(currCent>=25) {
	numQs = currCent/25;
	currCent = currCent-25*numQs;
	}
if(currCent>=10) {
	numDs = currCent/10;
	currCent = currCent-10* numDs;
	}
if(currCent>=5) {
	numNs = currCent/5;
	currCent = currCent-5*numNs;
	}
if(currCent>=1) {
	numPs = currCent/1;
	currCent = currCent-numPs;
	}
printf("$100 bills: \t %d \n", num100s);
printf("$50 bills: \t %d \n", num50s);
printf("$20 bills: \t %d \n", num20s);
printf("$10 bills: \t %d \n", num10s);
printf("$5 bills: \t %d \n", num5s);
printf("$1 bills: \t %d \n", num1s);
printf("Quarters: \t %d \n", numQs);
printf("Dimes:    \t %d \n", numDs);
printf("Nickels: \t %d \n", numNs);
printf("Pennies: \t %d \n", numPs);
}
	
