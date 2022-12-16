all: slow fast1 fast2

slow: main.c connectfour.c
	gcc main.c connectfour.c -o main_slow

fast1: main.c connectfour.c
	gcc -Ofast main.c connectfour.c -o main_fast1

fast2: main.c connectfour.c
	gcc -Ofast -funroll-loops -fstrict-aliasing main.c connectfour.c -o main_fast2
