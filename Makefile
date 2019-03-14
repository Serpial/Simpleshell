all:
	gcc -std=c99 main.c history.c alias.c execute.c -o out
run:
	gcc -std=c99 main.c history.c alias.c execute.c -o out
	./out
	rm out
