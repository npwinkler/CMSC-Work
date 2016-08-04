CC = gcc
CFLAG = -Wall
CFLAGS = -Wall -c
MOBS = bar.o foo.o
BOBS = bar.c bar.h
FOBS = foo.c foo.h bar.h
O = -o
MAIN = main
BARC = bar.c
BARO = bar.o
FOOC = foo.c
FOOO = foo.o
RMWF = rm -f

main: $(MOBS)
	$(CC) $(CFLAG) $(MOBS) $(O) $(MAIN)

bar.o: $(BOBS)
	$(CC) $(CFLAGS) $(BARC) $(O) $(BARO)

foo.o: $(FOBS)
	$(CC) $(CFLAGS) $(FOOC) $(O) $(FOOO)

clean:
	$(RMWF) $(FOOO) $(BARO) $(MAIN)


