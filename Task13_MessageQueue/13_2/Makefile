CC = gcc
debug = -g 
targetS = 13_2_server
targetC = 13_2_client
targetG = 13_2_graphic

COMPMAKE = make
REMOVE = rm -rf *.o $(targetS) $(targetC) $(targetG)

all: $(targetS) $(targetC)
	@echo Done!

$(targetS): $(targetS).o
	$(CC) $(targetS).o -o $(targetS)

$(targetC): $(targetC).o $(targetG).o
	$(CC) $(targetC).o $(targetG).o -o $(targetC) -lncurses

*.o: *.c
	$(CC) $(debug) -c *.c

clear:
	$(REMOVE)