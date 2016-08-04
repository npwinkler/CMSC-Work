
CS154 Project 5: IM Server

This directory should contain the following:

main.c: Start of code for main() function of the "ims" server, which
        you will complete for this project.  This code uses getopt()
        for handling the command-line options.

Makefile: Starter Makefile for "ims". Please modify as you need, according
          to whatever new source files you add to your project (so that
          the entirety of the server is not in main.c)

imp/: Header and library for handling protocol strings and protocol messages
 * imp.h: the IM protocol header file
 * libimp.so: a Linux shared library

db-empty.txt: A user database file with no registered users

db-example.txt: An example non-empty user database file

ims-ref: Reference IM server implementation. Example usage:
         cp db-example.txt db.txt
         ./ims-ref -p 8080 -d db.txt

txtimc: A very simple text-based IM client (a Linux executable)
txtimc.c: the single source for txtimc

README.txt: This file

The project was developed by the Computer Science department at the
University of Chicago.
