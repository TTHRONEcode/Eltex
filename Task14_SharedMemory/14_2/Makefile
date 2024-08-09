CC = gcc
compile_flags = -Werror -pedantic-errors -Wall -Wextra -Wpedantic
targetS = 14_2_server
targetC = 14_2_client
targetG = 14_2_graphic

COMPMAKE = make
REMOVE = rm -rf *.o $(targetS) $(targetC) $(targetG)

all: *.o $(targetS) $(targetC)
	@echo Done!

$(targetS): $(targetS).o
	$(CC) $(targetS).o -o $(targetS)

$(targetC): $(targetC).o $(targetG).o
	$(CC) $(targetC).o $(targetG).o  -o $(targetC) -lncurses

*.o: *.c
	$(CC) $(compile_flags) -c *.c

clear:
	$(REMOVE)
