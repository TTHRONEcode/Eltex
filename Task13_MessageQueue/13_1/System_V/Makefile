CC = gcc
debug = -g -Wall -Werror
targetS = 13_1_V_server
targetC = 13_1_V_client

COMPMAKE = make
REMOVE = rm -rf *.o $(targetS) $(targetC)

all: $(targetS) $(targetC)
	@echo Done!

$(targetS): 13_1_V_server.o
	$(CC) 13_1_V_server.o -o $(targetS)

$(targetC): 13_1_V_client.o
	$(CC) 13_1_V_client.o -o $(targetC)

*.o: *.c
	$(CC) $(debug) -c *.c

clear:
	$(REMOVE)