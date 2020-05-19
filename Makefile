LDLIBS=-lncursesw -lpthread
CFLAGS=-g

optimize: sudoku.c
	gcc -O3 sudoku.c -o sudoku

.PHONY: clean
clean:
	rm sudoku curs *.o
