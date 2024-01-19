FLAGS = -std=c17 -Wall -pedantic -Wdeprecated -Wextra

all: run

# main: main.o vm.o
# 	@clang $(FLAGS) -o $@ $^

# main.o: main.c
# 	@clang $(FLAGS) -c $^

# vm.o: vm.c vm.h
# 	@clang $(FLAGS) -c vm.c

foo2:
	@clang -g $(FLAGS) foo2.c -o foo2
	@./foo2

foo:
	@clang $(FLAGS) -g foo.c -o foo
	@objdump -S ./foo | tee ./foo.deassm

run: clean
	@clang -O3 $(FLAGS) main.c vm.c -o main
	@./main

debug:
	@clang -DZDX_TRACE_ENABLE -g $(FLAGS) main.c vm.c -o main-debug
	@./main-debug
	@objdump -S ./main-debug > ./main-debug.deassm

dyn_arr:
	@clang -O3 -Wall ./libs/dyn_array_test.c -o ./libs/dyn_array_test && leaks --atExit -- ./libs/dyn_array_test

dyn_arr_dbg:
	@clang -DZDX_TRACE_ENABLE -g -Wall -Wextra -Wdeprecated -pedantic ./libs/dyn_array_test.c -o ./libs/dyn_array_test && leaks --atExit -- ./libs/dyn_array_test

help:
	@echo "make <all run debug dyn_arr dyn_arr_dbg foo foo2 clean help>"

clean:
	$(RM) ./*.o ./*.dump ./*.deassm
	$(RM) -r ./**/*.dSYM
	$(RM) ./main ./foo ./foo{2,3} ./main-debug ./**/*_test ./**/*.out

.PHONY: all run debug dyn_arr dyn_arr_dbg foo foo2 clean help
