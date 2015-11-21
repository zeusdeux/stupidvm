all: run

main: main.c vm.c
	@echo "clang doing it's magic.."
	@clang -Wall -O3 $^ -o $@

runTrace: main
	@./main 1

run: main
	@./main

help:
	@echo "Phony targets: run, runTrace, help"
	@echo "Normal targets: main (deps: main.c vm.c)"

.PHONY: run runTrace help
