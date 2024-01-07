#ifndef VM_H_
#define VM_H_

#include <stdlib.h>

#define MAX_STACK_SIZE 1024
#define bail(...) {                                       \
    fprintf (stderr, "%s:%d: ", __FILE__, __LINE__);      \
    fprintf (stderr, __VA_ARGS__);                        \
    fprintf (stderr, "\n");                               \
    exit(1);                                              \
  }

#ifdef VM_TRACE_ENABLE
#define dbg(...) {                                        \
    fprintf (stderr, "%s:%d: ", __FILE__, __LINE__);      \
    fprintf (stderr, __VA_ARGS__);                        \
    fprintf (stderr, "\n");                               \
  }
#else
#define dbg(...) {}
#endif

typedef enum {
  HALT = 0,
  PUSH,
  IADD,
  PRINT,
  STORE,
  LOAD,
} Op;

typedef struct {
  Op opcode;
  union {
    int i_val;
    float f_val;
  };
} Instruction;

void run(const int program[]);

#endif // VM_H_
