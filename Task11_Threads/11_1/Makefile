CC = gcc
debug = -Wall -Werror
target = five_threads

$(target): *.o
	$(CC) *.o -o $(target)
	@echo Done!

*.o: *.c
	$(CC) $(debug) -c *.c

clear:
	rm -f *.o $(target)