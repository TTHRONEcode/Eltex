all: 
	gcc -c 7_1_main.c 7_1_add.c 7_1_sub.c 7_1_mul.c 7_1_div.c
	ar rc libcalc.a 7_1_add.o 7_1_sub.o 7_1_mul.o 7_1_div.o

	gcc 7_1_main.o -o calculator -L. -lcalc

	rm *.o