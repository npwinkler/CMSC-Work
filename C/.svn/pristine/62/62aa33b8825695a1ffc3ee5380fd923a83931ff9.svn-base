/* Nolan Winkler, nolanwinkler */
/* CS152, Summer 2013 */
/* Homework 1, Problem 1 */

#include <stdio.h>

/* The repetition of the printf statement makes for
   bulky code. Cleaner code would place the printf
   statement once at the end of the main function. */

int main(void) {
float income;
printf("Enter in your taxable income in the format e.g. $137.42, please.\t");
scanf("$%f", &income);
float tax;
if(income <= 650) {
    tax = .02 * income;
    printf("Tax Due: $%.2f\n", tax);
    }
else if(income <= 2375) {
    tax = 45.36 + .03 * (income - 650);
    printf("Tax Due: $%.2f\n", tax);
    }
else if(income <= 3239) {
    tax = 75.71 + .04 * (income - 2375);
    printf("Tax Due: $%.2f\n", tax);
    }
else if(income <= 6039) {
    tax = 123.12 + .05 * (income - 3239);
    printf("Tax Due: $%.2f\n", tax);
    }
else if(income <= 8412) {
    tax = 178.93 + .06 * (income - 6039);
    printf("Tax Due: $%.2f\n", tax);
    }
else {
    tax = 302.36 + .07 * (income - 8412);
    printf("Tax Due: $%.2f\n", tax);
    }
}