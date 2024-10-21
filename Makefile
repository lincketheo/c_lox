
clox: main.c utils.c string.c token.c scanner.c errors.c memory.c expression.c parser.c interpreter.c statements.c value.c value_hashtable.c
	gcc -o $@ $^ -g

.PHONY: format  
	
format:
	find . -iname '*.h' -o -iname '*.cpp' -o -iname '*.c' | xargs clang-format -i

expression_print_test: ./expression_print_test.c ./expression.c errors.c
	gcc -o $@ $^ -g

.PHONY: clean

clean:
	rm -f clox
