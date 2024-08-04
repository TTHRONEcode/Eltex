CC = gcc
debug = -g  #-Wall -Werror
targetS = 14_1_v_server
targetC = 14_1_v_client

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