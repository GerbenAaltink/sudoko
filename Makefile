all: build_and_run

build_and_run:
	gcc sudoku2.c -o sudoku -Wall -Wextra -pedantic
	./sudoku

coverage:
	gcc -pg -fprofile-arcs -ftest-coverage -g -o sudoku sudoku2.c
	./sudoku
	lcov --capture --directory . --output-file sudoku.coverage.info
	genhtml sudoku.coverage.info --output-directory sudoku.coverage
	@rm -f *.gcda   2>/dev/null
	@rm -f *.gcno   2>/dev/null
	@rm -f sudoku.coverage.info 2>/dev/null
	@rm -f gmon.out 2>/dev/null
	google-chrome sudoku.coverage/index.html