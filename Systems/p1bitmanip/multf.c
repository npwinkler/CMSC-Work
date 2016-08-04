/*
 * multFiveEighths - multiplies by 5/8 rounding toward 0.
 *   Should exactly duplicate effect of C expression (x*5/8),
 *   including overflow behavior.
 *   Examples: multFiveEighths(77) = 48
 *             multFiveEighths(-22) = -13
 *             multFiveEighths(1073741824) = 13421728 (overflow)
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 3
 */

#include <stdio.h>

int multFiveEighths(int x);

int main(){
	int value;	
	printf("Enter in your argument please.\n");
	scanf("%d", &value);
	printf("The result is %d\n", multFiveEighths(value));
	return 0;
}

int multFiveEighths(int x) {
  int fivex = x + x + x + x + x;
  printf("fivex is %d\n", fivex);
  int y = fivex >> 31;
  printf("sign of fivex is %d\n", y);  
return (fivex + (y & (8 + ~0))) >> 3;
}
