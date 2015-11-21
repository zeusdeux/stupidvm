#ifndef VM_H_
#define VM_H_

#define STUPIDVMMARKER 0xB055BABE
#define MAX_STACK_SIZE 1000

void run(const int *code, int startingAddr, int trace);

#endif
