/* Nolan Winkler, nolanwinkler */
/* CS152, Summer 2013 */
/* Lab 1 */

#include <stdio.h>

int main(void) {
	float oldTemp;
	char scale;
	float newTemp;
	printf("Enter in a temperature scale (f or c), please.");
	scanf("%c",&scale);
	if(scale == 'f')
		{
		printf("Enter in a temperature, please.\n");	
		scanf("%f",&oldTemp);
		newTemp = 5.0f/9.0f * (oldTemp - 32.0f);
		printf("%f F = %f C\n", oldTemp, newTemp);
		}
	else if(scale == 'c') 
		{
		printf("Enter in a temperature, please.\n");	
		scanf("%f",&oldTemp);
		newTemp = 32.0f + (9.0f/5.0f * oldTemp);
		printf("%f C = %f F\n", oldTemp, newTemp);
		}
	else
		{ 
		printf("Invalid temperature scale.\n");
		}
	}
