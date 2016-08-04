#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

void myPrint(char *msg)
{
    write(STDOUT_FILENO, msg, strlen(msg));
}

int main(int argc, char *argv[]) 
{
    char cmd_buff[100];
    char *pinput;

    while (1) {
        myPrint("myshell> ");
        pinput = fgets(cmd_buff, 100, stdin);
        if (!pinput) {
            exit(0);
        }
        myPrint(cmd_buff);
    }
}
