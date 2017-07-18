OBJS = main.o bstrlib.o parse.o handlers.o simclist.o threads.o sockets.o channel.o
DEPS = $(OBJS:.o=.d)
CC = gcc
CFLAGS = -I../../include -g3 -Wall -fpic -std=gnu99 -MMD -MP -DDEBUG
BIN = ../chirc
LDLIBS = -l pthread

all: $(BIN)
	
$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN) $(LDLIBS)
	
%.d: %.c

clean:
	-rm -f $(OBJS) $(BIN) *.d
