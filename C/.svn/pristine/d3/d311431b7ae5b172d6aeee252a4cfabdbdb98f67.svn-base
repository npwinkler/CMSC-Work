/* Nolan Winkler, nolanwinkler */
/* CS152, Summer 2013 */
/* Homework 1, Problem 3 */

#include <stdio.h>

/* There is an excess of if statements in this solution.
   In the future, think about how you could combine some
   of these statements using logical operators for a
   cleaner solution. */

int main(void) {
int mm1, dd1, yy1, mm2, dd2, yy2;
printf("Enter first date (mm/dd/yy):");
scanf("%02d/%02d/%02d",&mm1,&dd1,&yy1);
printf("Enter second date (mm/dd/yy):");
scanf("%02d/%02d/%02d",&mm2,&dd2,&yy2);
if(yy1==yy2&&mm1==mm2&&dd1==dd2) {
	printf("%02d/%02d/%02d is the same as %02d/%02d/%02d\n",
	mm1,dd1,yy1,mm2,dd2,yy2);
	}
else if(yy1 < yy2) {
	printf("%02d/%02d/%02d is earlier than %02d/%02d/%02d\n",
	mm1,dd1,yy1,mm2,dd2,yy2);
	}
else if(yy1 > yy2) {
	printf("%02d/%02d/%02d is earlier than %02d/%02d/%02d\n",
	mm2,dd2,yy2,mm1,dd1,yy1);
	}
else if (mm1 < mm2) {
	printf("%02d/%02d/%02d is earlier than %02d/%02d/%02d\n",
	mm1,dd1,yy1,mm2,dd2,yy2);
	}
else if(mm1 > mm2) {
	printf("%02d/%02d/%02d is earlier than %02d/%02d/%02d\n",
	mm2,dd2,yy2,mm1,dd1,yy1);
	}
else if (dd1 < dd2) {
	printf("%02d/%02d/%02d is earlier than %02d/%02d/%02d\n",
	mm1,dd1,yy1,mm2,dd2,yy2);
	}
else if(dd1 > dd2) {
	printf("%02d/%02d/%02d is earlier than %02d/%02d/%02d\n",
	mm2,dd2,yy2,mm1,dd1,yy1);
	}
}
	
