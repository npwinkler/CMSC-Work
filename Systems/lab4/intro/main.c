#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>

/* Adapted from http://randu.org/tutorials/threads/ */

#define NUM_THREADS 5

/* returns current time (since the "Epoch") as double */
double dtime(void) {
  double sec, usec;
  struct timeval tv;

  gettimeofday(&tv, NULL);
  sec = (double)tv.tv_sec;
  usec = (double)tv.tv_usec;
  return sec + usec*1.0e-6;
}

/* create thread argument struct for thr_func();
   Nothing very interesting here; its mainly to demonstrate
   how things are passed to the pthread "start routine"
   (3rd argument to pthread_create) */
typedef struct {
  int index;
  pthread_t tid;
} thread_data;

/* thread function */
void *thr_func(void *_data) {
  thread_data *data;
  double tt, *when;

  /* to demonstrate a return value from a thread; we create a space to
     store a return value (in this case, the current time), and then
     return a pointer to that space.  Our code could just as well have
     used space pre-allocated by main: nothing about the value returned
     via pthread_exit is required to be dynamically allocated */
  tt = dtime();
  when = calloc(1, sizeof(double));
  when[0] = tt;

  /* because all data is passed around in pthreads as a void*,
     its up to you to cast it as you need */
  data = (thread_data *)_data;

  /* pthread_self() returns the same as what was learned through the
     pthread_create() call. Note that pthread_t is not actually
     guaranteed to be an arithmetic type at all, so casting to
     unsigned int is not strictlyl speaking portable */
  data->tid = pthread_self();
  printf("thr_func(%u): hello from thread[%d]\n",
	 (unsigned int)data->tid, data->index);

  /* the return value is passed (by pointer) here */
  pthread_exit(when);
}

int main(int argc, char **argv) {
  pthread_t thr[NUM_THREADS];
  int i, rc;
  /* create a thread_data argument array; each one will
     be passed as the sole argument to thr_func */
  thread_data thr_data[NUM_THREADS];
  double start;

  start = dtime();
  /* create threads */
  for (i = 0; i < NUM_THREADS; ++i) {
    thr_data[i].index = i;
    if ((rc = pthread_create(&(thr[i]), NULL, thr_func, &(thr_data[i])))) {
      fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
      exit(1);
    }
    printf("main: thr[%d] is pthread %u\n", i, (unsigned int)thr[i]);
  }

  /* block until all threads complete */
  for (i = 0; i < NUM_THREADS; ++i) {
    void *ret;
    double when;
    pthread_join(thr[i], &ret);
    /* casting from void* again */
    when = *(double *)(ret);
    free(ret); //added to prevent against memory leak
    printf("main: thread %d started running after %f seconds\n",
	   i, when - start);
  }

  exit(0);
}
