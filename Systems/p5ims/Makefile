
CC=gcc
CFLAGS= -Wall -Werror -g -O1
IPATH= -Iimp
LPATH= -Limp

OBJS= main.o

%.o: %.c
	$(CC) $(CFLAGS) $(IPATH) -c $< -o $@

ims: $(OBJS)
	$(CC) $(CFLAGS) $(IPATH) -o ims $(OBJS) $(LPATH) -Wl,-rpath,imp -lpthread -limp

clean:
	rm -f *.o *~ ims

