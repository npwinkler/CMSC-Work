#include <stdio.h>
#include <pthread.h>

const int nEntries = 10000;
int var = 0;

void incrementVar() {
  var = var+1;
}

void* child_fn(void* arg) {
  int i;

  for (i=0; i<nEntries; i++) {
    incrementVar();
  }
  return NULL;
}

int main() {
  pthread_t child;
  int i=0;

  printf("&var = %p\n", &var);
  pthread_create(&child, NULL, child_fn, NULL);

  for (i=0; i<nEntries; i++) {
    incrementVar();
  }

  pthread_join(child, NULL);

  printf("var = %d\n", var);
  return 0;
}
