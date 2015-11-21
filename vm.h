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

enum Instruction {
  INUM = 1, // 1
  IADD,     // 2
  IMUL,     // 3
  ISTORE,   // 4
  ILOAD,    // 5
  STR,      // 6
  POP,      // 7
  IPRINT,   // 8
  IPRINTLN, // 9
  SPRINT,   // 10
  SPRINTLN, // 11
  JMP,      // 12
  JMPF,     // 13
  JMPT,     // 14
  JMPLT,    // 15
  JMPLTE,   // 16
  JMPGT,    // 17
  JMPGTE,   // 18
  JMPEQ,    // 19
  JMPNE,    // 20
  HALT      // 21
};

extern const char *ins[];

void run(const int *code, int startingAddr, int trace);

#endif
