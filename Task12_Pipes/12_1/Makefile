CC = gcc
debug = -Wall -Werror
target = fildes_Hi

COMPMAKE = make
MAKEPATH = ../../Task10_Proc/
REMOVE = rm *.o ./*

$(target): *.o
	cd $(MAKEPATH) && $(COMPMAKE) 
	$(CC) $(MAKEPATH)*.o *.o -o $(target)
	@echo Done!

*.o: *.c
	$(CC) $(debug) -c *.c

clear:
	rm -f *.o $(target)

clear_all:
	rm -f *.o $(target)
	cd $(MAKEPATH) && rm -f *.o