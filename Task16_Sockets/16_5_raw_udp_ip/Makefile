CC = gcc
compile_flags = -g -O2 -Werror -pedantic-errors -Wall -Wextra -Wpedantic
targetS = 16_5_raw_udp_ip_serv
targetC = 16_5_raw_udp_ip_client

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
