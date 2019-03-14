all:
	gcc -std=c99 main.c phrase.c history.c alias.c execute.c -o out
run:
	gcc -std=c99 main.c phrase.c history.c alias.c execute.c -o out
	./out
	rm out
