all: slow fast

slow: main.c connectfour.c
	gcc main.c connectfour.c -o main_slow

fast: main.c connectfour.c
	gcc -Ofast main.c connectfour.c -o main_fast
