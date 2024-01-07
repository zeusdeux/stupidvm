FLAGS = -std=c17 -Wall -pedantic -Wdeprecated -Wextra

all: run

# main: main.o vm.o
# 	@clang $(FLAGS) -o $@ $^

# main.o: main.c
# 	@clang $(FLAGS) -c $^

# vm.o: vm.c vm.h
# 	@clang $(FLAGS) -c vm.c

foo:
	@clang -g $(FLAGS) -c foo.c # generate foo.o
	@clang -g $(FLAGS) foo.o -o foo
	@objdump -S ./foo | tee ./foo.deassm

run: clean
	@clang -O3 $(FLAGS) main.c vm.c -o main
	@./main

debug:
	@clang -DVM_TRACE_ENABLE -ggdb $(FLAGS) main.c vm.c -o main-debug
	@./main-debug
	@objdump -S ./main-debug > ./main-debug.deassm

help:
	@echo "make <run, debug, foo, help, clean>"

clean:
	$(RM) ./*.o ./*.dump ./*.deassm
	$(RM) -r ./*.dSYM
	$(RM) ./main ./foo ./main-debug

.PHONY: all run debug foo clean help
