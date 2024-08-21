CC = gcc
compile_flags = -Werror -pedantic-errors -Wall -Wextra -Wpedantic
targetS = 16_2_2_udp_inet_server_main
targetS_SUB = 16_2_2_udp_inet_server_sub
targetC = 16_2_2_udp_inet_client

COMPMAKE = make
REMOVE = rm -rf *.o $(targetS) $(targetC) $(targetS_SUB) 

all: *.o $(targetS) $(targetC) $(targetS_SUB) 
	@echo Done!

$(targetS): $(targetS).o
	$(CC) $(targetS).o -o $(targetS)

$(targetC): $(targetC).o
	$(CC) $(targetC).o -o $(targetC)

$(targetS_SUB): $(targetS_SUB).o
	$(CC) $(targetS_SUB).o -o $(targetS_SUB)

*.o: *.c
	$(CC) $(compile_flags) -c *.c

clear:
	$(REMOVE)
