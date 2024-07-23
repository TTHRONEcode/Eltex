CC = gcc
debug = -g -Wall -Werror
target = totalblack
libs = -lmenu -lncurses

COMPMAKE = make
REMOVE = rm *.o ./*

$(target): *.o
	cd .. && $(COMPMAKE) 
	$(CC) ./../*.o *.o -o $(target) $(libs)
	@echo Done!

*.o: *.c
	$(CC) $(debug) -c *.c

clear:
	rm -f *.o $(target)
	
clear_all:
	rm -f *.o $(target)
	cd .. && rm -f *.o
	 
