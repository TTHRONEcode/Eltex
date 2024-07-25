CC = gcc
debug = -g -Wall -Werror
target = shoping

$(target): *.o
	$(CC) *.o -o $(target)
	@echo Done!

*.o: *.c
	$(CC) $(debug) -c *.c

clear:
	rm -f *.o $(target)