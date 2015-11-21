FLAGS = -Wall -O3

all: run

main: main.o vm.o
	@clang $(FLAGS) -o $@ $^

main.o: main.c
	@clang $(FLAGS) -c $^

vm.o: vm.c vm.h
	@clang $(FLAGS) -c vm.c

runTrace: main
	@./main 1

run: main
	@./main

help:
	@echo "Phony targets: run, runTrace, help"
	@echo "Normal targets: main (deps: main.c vm.c)"

clean:
	$(RM) ./*.o
	$(RM) ./main

.PHONY: run runTrace help clean
