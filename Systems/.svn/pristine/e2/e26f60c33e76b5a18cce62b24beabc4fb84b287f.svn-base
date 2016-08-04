/*
 * Just a simple test program for playing with gdb.
 */

#include <stdio.h>
#include <stdlib.h>

int compute(int);

int main (int argc, char **argv)
{
  int i = 10;
  int result = 0;

  result = compute(i);

  printf("%d\n", result);

  return 0;
}
  

int compute(int i) {
  int result = 0;
  
  while (i > 0)
  {
    i--;
    
    if (i % 2 == 0)
    {
      result *= i;
    }
    else
    {
      result += i;
    }
  }

  return result;
}
