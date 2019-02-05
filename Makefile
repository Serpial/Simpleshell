all:
	gcc main.c -o out
run:
	gcc main.c -o out
	./out
	rm out
