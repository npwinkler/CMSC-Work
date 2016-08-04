main: bar.o foo.o
	gcc -Wall bar.o foo.o -o main

bar.o: bar.c bar.h
	gcc -Wall -c bar.c -o bar.o

foo.o: foo.c foo.h bar.h
	gcc -Wall -c foo.c -o foo.o

clean:
	rm -f foo.o bar.o main


