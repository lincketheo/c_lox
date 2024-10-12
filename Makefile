
jlox: main.c utils.c string.c token.c scanner.c errors.c memory.c
	gcc -o $@ $^ -g

format:
	find . -iname '*.h' -o -iname '*.cpp' -o -iname '*.c' | xargs clang-format -i

expression_print_test: ./expression_print_test.c ./expression.c errors.c
	gcc -o $@ $^ -g
