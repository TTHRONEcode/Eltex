CC = gcc
compile_flags = -Werror -pedantic-errors -Wall -Wextra -Wpedantic
targetS = 16_2_1_tcp_inet_server
targetC = 16_2_1_tcp_inet_client

COMPMAKE = make
REMOVE = rm -rf *.o $(targetS) $(targetC)

all: *.o $(targetS) $(targetC)
	@echo Done!

$(targetS): $(targetS).o
	$(CC) $(targetS).o -o $(targetS)

$(targetC): $(targetC).o 
	$(CC) $(targetC).o -o $(targetC)

*.o: *.c
	$(CC) $(compile_flags) -c *.c

clear:
	$(REMOVE)
