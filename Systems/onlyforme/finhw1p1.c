#include <stdio.h>

int main() {
  int endian, foo;
  foo = ~255;
  endian = *((char *) (&foo));
  printf("hello world, from a %s-endian machine\n",
         endian ? "big" : "little");
  return endian;
}
