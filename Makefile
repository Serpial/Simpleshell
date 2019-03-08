all:
	gcc -std=c99 main.c -o out
run:
	gcc -std=c99 main.c -o out
	./out
	rm out
