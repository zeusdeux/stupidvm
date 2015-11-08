main: main.c
	@echo "clang doing it's magic.."
	@clang -Wall $^ -o $@
