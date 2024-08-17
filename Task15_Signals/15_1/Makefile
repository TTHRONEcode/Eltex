comp_flags = -Werror -pedantic-errors -Wall -Wextra -Wpedantic
target = sigaction
dependence = 15_1_main

$(target): $(dependence).c
	gcc $(comp_flags) $(dependence).c -o $(target)

clear:
	rm -f $(target)