comp_flags = -Werror -pedantic-errors -Wall -Wextra -Wpedantic
target = sigwait
dependence = 15_2_main

$(target): $(dependence).c
	gcc $(comp_flags) $(dependence).c -o $(target)

clear:
	rm -f $(target)