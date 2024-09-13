CC = gcc
CFLAGS = -O2 -g -Werror -pedantic-errors -Wall -Wextra -Wpedantic

src_server = *serv*.c
src_client = *client.c
src_header = *.h
src_makefile = Makefile

target_server = Server
target_client = Client

format_path_file = "$(<D)/$(<F)"

.PHONY: all
all: $(target_server) $(target_client) $(src_makefile)
	@printf "%3s complete\n" "BLD"

$(target_server): $(src_server) $(src_header) $(src_makefile)
	@printf "%3s %s > %s\n" "CC" $(format_path_file) $(target_server)
	@$(CC) $(CFLAGS) $(src_server) -o $@

$(target_client): $(src_client) $(src_header) $(src_makefile)
	@printf "%3s %s > %s\n" "CC" $(format_path_file) $(target_client)
	@$(CC) $(CFLAGS) $(src_client) -o $@

.PHONY: clean
clean:
	@printf "%3s *.o ./*\n" "CLR"
	@rm -f *.o $(target_client) $(target_server)

.PHONY: mrproper
mrproper:
	#rm -rf / #HAHAHA
