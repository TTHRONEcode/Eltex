CC = gcc
debug = -Wall -Werror
target = 10_1_proc

COMPMAKE = make
REMOVE = rm *.o ./*

$(target): *.o
	cd .. && $(COMPMAKE) 
	$(CC) ./../*.o *.o -o $(target)
	@echo Done!

*.o: *.c
	$(CC) $(debug) -c *.c

clear:
	rm -f *.o $(target)

clear_all:
	rm -f *.o $(target)
	cd .. && rm -f *.o