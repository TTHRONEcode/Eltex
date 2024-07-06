all: 
	gcc -fPIC -c 8_1_add.c 8_1_sub.c 8_1_mul.c 8_1_div.c
	gcc -shared 8_1_add.o 8_1_sub.o 8_1_mul.o 8_1_div.o -o libcalc.so
	gcc 8_1_main.c -o calculator -L. -lcalc
	rm *.o
	# LD_LIBRARY_PATH=$PWD:$LD_LIBRARY_PATH ./calculator
