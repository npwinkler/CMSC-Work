CC = gcc -m32
CFLAGS = -Wall -g

test: test.o library.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f test
