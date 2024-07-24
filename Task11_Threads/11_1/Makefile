CC = gcc
debug = -Wall -Werror
target = five_threads

COMPMAKE = make
REMOVE = rm *.o ./*

$(target): *.o
	$(CC) *.o -o $(target)
	@echo Done!

*.o: *.c
	$(CC) $(debug) -c *.c

clear:
	rm -f *.o $(target)