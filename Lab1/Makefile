CXXFLAGS+=-O2 -ggdb -DDEBUG
CXXFLAGS+=-Wall -Wextra


sudoku_solve: main.o neighbor.o sudoku_basic.o sudoku_min_arity.o sudoku_min_arity_cache.o sudoku_dancing_links.o 
	g++ main.o neighbor.o sudoku_basic.o sudoku_min_arity.o sudoku_min_arity_cache.o sudoku_dancing_links.o  -lpthread -o sudoku_solve

clean: 
	rm *.o
	