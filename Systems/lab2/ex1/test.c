/*
 * Exercise 1 - do not modify this file! "Test runner."
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int multiStrlen(char *first, char *second);

void test1()
{
    int result = multiStrlen("ab", "cd");
    if (result != 4) {
        printf("test 1 failed with result %d.\n", result);
    } else {
        printf("test 1 passed.\n");
    }
}

char *twoFingerSort(char *first, char *second);
void test2()
{
    char *result = twoFingerSort("dcba", "aafa");
    
    if (strcmp(result, "fdcbaaaa") != 0) {
        printf("test 2 failed with result %s.\n", result);
    } else {
        printf("test 2 passed.\n");
    }

    free(result);
}


int main (int argc, char **argv)
{
    test1();
    test2();

    return 0;
}
