/*
 * Just a simple test program for playing with gdb.
 */

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char **argv)
{
  int offset = 0;
  int i;
  char data[] = "Some text.";
  char stringToPrint[] = "Some additional text.";
  char data3[] = "A final bit of text.";
  char data4[] = "Well, maybe just a little bit more text, just for fun!";

  for (i = 0; i < 10; i++) {
    data[i] = data4[i]+1;
  }

  for (i = 13; i < 26; i++) {
    data4[i*2] = data4[i];
  }

  for (i = 200; i > 13; i--) {
    offset = i/2;
    data4[offset] = data4[i];
  }

  printf("%s\n", stringToPrint);
  return 0;
}
