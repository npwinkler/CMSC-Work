#include <stdio.h>
#include <pthread.h>

typedef struct lv {
    pthread_mutex_t lock;
    int value;
} LockedValue;

const int nEntries = 400;
LockedValue odd, even;

void* upper_fn(void* arg) {
  int i = 0;

  for (i = nEntries/2; i < nEntries; i++) {
    pthread_mutex_lock (&odd.lock);

    if (i % 2 == 1) {
      odd.value++;
    } else {
      pthread_mutex_lock (&even.lock);
      even.value++;
      pthread_mutex_unlock (&even.lock);
    }

    pthread_mutex_unlock (&odd.lock);
  }

  return NULL;
}

void* lower_fn(void* arg) {
  int i = 0;

  for (i=0; i<nEntries/2; i++) {
    pthread_mutex_lock (&even.lock);

    if (i % 2 == 0) {
      even.value++;
    } else {
      pthread_mutex_lock (&odd.lock);
      odd.value++;
      pthread_mutex_unlock (&odd.lock);
    }

    pthread_mutex_unlock (&even.lock);
  }

  return NULL;
}

int main() {
  pthread_t upperThread, lowerThread;

  printf("starting counting (^C if not finished in a second) ...\n");
    /* this programs counts the odd and even numbers between
       0 and nEntries-1 in a complicated way */

  pthread_mutex_init(&(odd.lock), NULL);
  pthread_mutex_init(&(even.lock), NULL);

  printf(" ... &(odd.lock)=%p; &(even.lock)=%p ...\n",
	 &(odd.lock), &(even.lock));

  pthread_create(&upperThread, NULL, upper_fn, NULL);
  pthread_create(&lowerThread, NULL, lower_fn, NULL);

  pthread_join(upperThread, NULL);
  pthread_join(lowerThread, NULL);

  pthread_mutex_destroy(&(odd.lock));
  pthread_mutex_destroy(&(even.lock));

  printf ("... %d odds, %d evens\n", odd.value, even.value);
  return 0;
}
