CC = gcc
debug = -g -Wall -Werror
target = 12_3_proc_mk2

COMPMAKE = make
REMOVE = rm *.o ./*
MAKEPATH = ../../Task10_Proc/

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