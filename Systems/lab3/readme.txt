nolanwinkler

/* Exercise 0 */
==19066== LEAK SUMMARY:
==19066==    definitely lost: 4,884 bytes in 71 blocks
==19066==    indirectly lost: 1,748 bytes in 13 blocks
==19066==      possibly lost: 44,001 bytes in 128 blocks
==19066==    still reachable: 51,501 bytes in 1,001 blocks
==19066==         suppressed: 0 bytes in 0 blocks

No, valgrind found more problems - 
==19066== ERROR SUMMARY: 6 errors from 1 contexts (suppressed: 2 from 2)
This also seems questionable: 
==19066== Conditional jump or move depends on uninitialised value(s)

/* Exercise 1 */
1. In all cases,
==20395== 1 errors in context 1 of 1:
==20395== Conditional jump or move depends on uninitialised value(s)
==20395==    at [somewhere]: fun[n] (vgme.c:[somewhere])
==20395==    by 0x400B24: main (vgme.c:136)
2. Function 0 is different from the other two because it uses an array created on the stack
whereas the other two use an array created on the heap
3. Make is now recognizing the unitialization problem althrough Valgrind also still is 

4. No error is reported in fun3 by Valgrind. I do not know if we've learned in class, but I'd imagine
and it seems that Valgrind reports errors of global variables explicitly made as pointers, but not
global variables made as arrays. So, right now as a mental note, I think that Valgrind just has trouble
with global arrays but not with global pointers, but it might be that it also just has trouble with
arrays on the stack rather than pointers/arrays on the heap in general, which would make sense that we
would have a project that forces us to get good at malloc. 

/* Exercise 2 */
1. in fun4, the x[SIZE] array is on the stack whereas in fun5 the x[SIZE] array is on the heap as indicated
by the malloc.
2. fun4 gives us a seg fault while fun5 doesn't. However, the bug in both is that X[SIZE] is out of bounds of the
array x of size SIZE which goes from x[0] to x[SIZE-1].

3. The results of running these two functions under Valgrind are so different because in 4 even though we have a segmentation fault
slash stack overflow it doesn't really tell us where this happened and is written more confusingly and the error summary shows 0. 
For 5, it shows us where we wrote and read wrong, that our mallloc clearly did not work as we weanted it to and gives us references to the line numbers in our program, which is obviously better. This just goes to show again that it's better to work with mallocs for arrays at least of large length. 

4. There are two errors for the line in fun5 because there is both a read and a write error. We first try to write 154 to x[SIZE], which is out of bounds so there is a write error. We then try to read x[SIZE], which is out of bounds so there is a read error, and write it to sink.
5. Yes there is reason to have an analogous testing function that uses gxd instead of gxs because gxd is a pointer and clearly Valgrind is better when that is true.
6. Valgrind does not detect the error at all in this case. This is due to gxs being an array on the stack rather than on the heap. DECLARE ARRAYS BY MALLOCING THEM ONTO THE HEAP. USE POINTERS.

/* Exercise 3 */
1. The bug in fun7 is that the array is freed before we attempt to access it, so we don't know how to access x[42].
2. Valgrind reports the out of bounds error in fun5 by saying the address is 0 bytes after the block, whereas in fun7 Valgrind tells us the array has already been freed as the line number it points to is free not malloc and it also tells us that we were trying to access an address 168 bytes into our array, which should have been fine if we had not already freed it. 
3. Looking at what SIZE is, 120, in the source code and the fact that we did malloc SIZE * sizeof(int) and then Valgrind tells us that the number of bytes allocated in our block was 480, we can deduce sizeof(int) = 4.

/* Exercise 4 */
1. The only free() statement that didn't generate a valgrind error is the one on line 88, free(same(x)).
2. the first free is attempting to access main memory directly
the second free is attempting to access the heap
the third free is attempting to acess the text segment of memory
the fourth free is attempting to access the data segment of memory
the sixth free is attempting to acess the heap, but doesn't know where to go
3. When we remove -g, the line numbers in vgme.c no longer get displayed, rather it just says it comes from that file
4. Same was introduced so that when we free things, we make sure we are freeing things on the heap i.e. we are making sure that the arrays we are freeing are being detected by Valgrind as being on the heap?

/* Exercise 5 */
1. The bug is that we never free our array so that when we exit the program that memory is still allocated. 
These lines let us know that:
==27557== HEAP SUMMARY:
==27557==     in use at exit: 480 bytes in 1 blocks /* still 1 block we never freed */
==27557==   total heap usage: 2 allocs, 1 frees, 960 bytes allocated /* allocated 2x but only freed 1x */
==27557== 
==27557== LEAK SUMMARY:
==27557==    definitely lost: 480 bytes in 1 blocks /* definitely lost this guy */

2. Yes it does isolate exactly wher the memory bug is because it gives us vgme.c:95 in fun9 which shows us which array we malloced is the one that never got freed. 

