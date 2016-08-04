#include <stdio.h>
#include <stdlib.h>

#define SIZE 120

char *me=NULL;
int sink;
int gxs[SIZE];
int *gxd;

/*
** returns the same pointer as passed, but in a way that an
** optimizing compiler can't recognize
*/
void *same(void *ptr) {
  char buff[128];
  long long int pi;

  sprintf(buff, "%lld", (long long int)ptr);
  sscanf(buff, "%Ld", &pi);
  return (void*)pi;
}

void fun0() {
  int y, x[SIZE];
  y = x[0];
  printf("%s\n", y > 0 ? "foo" : "bar");
  return;
}

void fun1() {
  int y, *x;
  x = (int*)malloc(SIZE*sizeof(int));
  y = x[0];
  printf("%s\n", y > 0 ? "foo" : "bar");
  free(x);
  return;
}

void fun2() {
  int y;
  y = gxd[0];
  printf("%s\n", y > 0 ? "foo" : "bar");
  return;
}

void fun3() {
  int y;
  y = gxs[0];
  printf("%s\n", y > 0 ? "foo" : "bar");
  return;
}

void fun4() {
  int x[SIZE];
  sink = x[SIZE] = 154;
  return;
}

void fun5() {
  int *x;
  x = (int*)malloc(SIZE*sizeof(int));
  sink = x[SIZE] = 154;
  free(x);
  return;
}

void fun6() {
  sink = gxs[SIZE] = 154;
  return;
}

void fun7() {
  int *x;
  x = (int*)malloc(SIZE*sizeof(int));
  free(x);
  sink = x[42] = 154;
  return;
}

void fun8() {
  int *x;
  x = (int*)malloc(SIZE*sizeof(int));
  free((void*)0x1540000);
  free(x + 42);
  free(same(fun1));
  free(same(gxs));
  free(same(x));
  free(x);
  return;
}

void fun9() {
  int *x;
  x = (int*)malloc(SIZE*sizeof(int));
  sink = x[42] = 154;
}

#define FUN_NUM 10

/* compile-time definition of the function vector */
void (*fun[FUN_NUM])(void) = {
  fun0, fun1, fun2, fun3, fun4, fun5, fun6, fun7, fun8, fun9
};

void usage(void) {
  /*             argv[]:  0  1  (2) */
  fprintf(stderr, "\nusage: %s N\n\n", me);
  fprintf(stderr, "where N is the number of the function to run; in [0,%d]\n",
	  FUN_NUM-1);
  return;
}

int main(int argc, char *argv[]) {
  int fidx;

  me = argv[0];
  if (2 != argc) {
    usage();
    return 1;
  }
  if (1 != sscanf(argv[1], "%d", &fidx)) {
    fprintf(stderr, "%s: couldn't parse %s as int\n", me, argv[1]);
    usage();
    return 1;
  }
  if (!( 0 <= fidx && fidx <= FUN_NUM-1 )) {
    fprintf(stderr, "%s: function index %d out of range [0,%d]\n",
	    me, fidx, FUN_NUM-1);
    usage();
    return 1;
  }
  gxd = (int*)malloc(SIZE*sizeof(int));

  /* call the chosen function */
  (fun[fidx])();

  free(gxd);
  return 0;
}
