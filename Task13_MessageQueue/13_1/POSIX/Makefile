CC = gcc
debug = -g -Wall -Werror
targetS = 13_1_POSIX_server
targetC = 13_1_POSIX_client

COMPMAKE = make
REMOVE = rm -rf *.o $(targetS) $(targetC)

all: $(targetS) $(targetC)
	@echo Done!

$(targetS): $(targetS).o
	$(CC) $(targetS).o -o $(targetS)

$(targetC): $(targetC).o
	$(CC) $(targetC).o -o $(targetC)

*.o: *.c
	$(CC) $(debug) -c *.c

clear:
	$(REMOVE)