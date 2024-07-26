CC = gcc
debug = -Wall -Werror
target = _hi

COMPMAKE = make
REMOVE = rm *.o ./*

$(target): *.o
	$(CC) *client.o -o client$(target)
	$(CC) *server.o -o server$(target)
	@echo Done!

*.o: *.c
	$(CC) $(debug) -c *.c

clear:
	rm -f *.o *$(target)
	rm -f fifo_path_server