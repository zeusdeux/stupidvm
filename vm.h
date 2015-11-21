#ifndef VM_H_
#define VM_H_

#define STUPIDVMMARKER 0xB055BABE
#define MAX_STACK_SIZE 1000

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef EINVALIDJMP
#define EINVALIDJMP "Invalid address to jump to"
#endif

#ifndef EINVALIDBC
#define EINVALIDBC "Invalid bytecode file"
#endif


void run(const int *code, int startingAddr, int trace);

#endif
