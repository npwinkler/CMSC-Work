nolanwinkler

Intro
1. The ordering of the output lines varies with each run because each thread can
take a different amount of time to run its code, so while "main:thr[i] is pthread
#" will always be printed before "main: thr[i+1]..." and "main: thread i started"
before "main: thread i+1..", once a thread is made it runs its code on its own
so it can print at any time with respect to the rest of the threads and main.

2. Data is passed to the start routine by the 4th argument of pthread_create, which
is always a void *. Start routine receives the data as a void * and then has to
cast it as whatever it needs it to be inside of its own function e.g. (thread_data *)

3. Thread functions return data to pthread_join by returning a pointer to the data
that is needed.

4. ==28620==     in use at exit: 40 bytes in 5 blocks
==28620==   total heap usage: 15 allocs, 10 frees, 3,014 bytes allocated
==28620== 
==28620== 8 bytes in 1 blocks are still reachable in loss record 1 of 2
==28620==    at 0x4C29DB4: calloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==28620==    by 0x400865: thr_func (main.c:42)
==28620==    by 0x4E39E99: start_thread (pthread_create.c:308)
==28620== 
==28620== 32 bytes in 4 blocks are definitely lost in loss record 2 of 2
==28620==    at 0x4C29DB4: calloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==28620==    by 0x400865: thr_func (main.c:42)
==28620==    by 0x4E39E99: start_thread (pthread_create.c:308)
free'ing the pointer returned by each thread after we get the value out of it in main
by adding free(ret) in the for loop that gets the results prevents against memory leak.

Exercise 1
The data race in the program is that both the child thread and the parent process
are incrementing the same global variable, var. 
Helgrind gives us the nice "possible  data race" at [variable location] clause 
and then shows us that both thread #1 andthread #2 are trying to acess it. It 
shows us where in the two threads the function that eventually increments var is
& where in that function var is being incremented.

The --read-var-info=yes option from the data race manual thing is nice, but makes
Helgrind run slower.

Exercise 2
==2101== Thread #3: lock order "0x6010C0 before 0x601080" violated
==2101== 
==2101== Observed (incorrect) order is: acquisition of lock at 0x601080
==2101==  followed by a later acquisition of lock at 0x6010C0
Thread #3 got the even lock before it got the odd lock, and the reverse is what's
supposed to happen because earlier a thread got the odd lock and then the even lock,
meaning that Thread #3 can get the even lock after the other thread got the odd lock
but before that other thread got the even lock, meaning that neither of them will
be able to get both locks and the same time and access the shared resource, instead
just hanging. 
/* remember acquiring a lock means locking a lock */
Parent is thread #1, Upper is thread #2, lower is thread #3
So, lower gets the odd lock first and then gets the even lock whereas upper gets
the even lock first and then gets the odd lock -> this causes a deadlock when
lower grabs the odd lock and upper grabs the even lock before lower can
