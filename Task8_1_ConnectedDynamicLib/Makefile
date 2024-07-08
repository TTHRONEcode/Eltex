all: 
	gcc -g -c -fPIC 8_1_1_add.c 8_1_1_sub.c 8_1_1_mul.c 8_1_1_div.c
	gcc -shared -o libcalc_add.so 8_1_1_add.o 
	gcc -shared -o libcalc_sub.so 8_1_1_sub.o 
	gcc -shared -o libcalc_mul.so 8_1_1_mul.o 
	gcc -shared -o libcalc_div.so 8_1_1_div.o 
	
	gcc -g 8_1_1_main.c -o calculator -ldl
	rm *.o