#include <stdio.h>
#include "vm.h"

char *Ops[] = {
  "HALT",
  "PUSH",
  "IADD",
  "PRINT",
  "STORE",
  "LOAD",
};

void run(const int program[])
{
  const size_t num_instructions = program[0];
  const size_t data_segment_size = program[1];

  void* data[data_segment_size];
  void* stack[MAX_STACK_SIZE];
  (void) data;

  int sp = -1; // stack pointer
  size_t ip = 2; // 0 -> number of instructions, 1 -> data segment size

  while (ip < num_instructions + 2) {
    // decode
    unsigned int op = program[ip++];

    // execute
    switch(op) {
    case PUSH: {
      dbg("%s %u", Ops[op], program[ip]);
      stack[++sp] = (void *)&program[ip++];
      dbg("stack[%d]: value -> %d pointer -> %p", sp, *(int *)stack[sp], stack[sp]);
    } break;
    case IADD: {
      int *b = (int*)stack[sp--];
      int *a = (int*)stack[sp--];
      int *result = (int *)malloc(sizeof(int));
      *result = *a + *b;

      dbg("%s %d %d", Ops[op], *a, *b);

      if (a > &program[num_instructions + 1] || a < &program[0]) {
        free(a);
      }

      if (b > &program[num_instructions + 1] || b < &program[0]) {
        free(b);
      }

      stack[++sp] = result;
      dbg("stack[%d]: value -> %d pointer -> %p", sp, *(int *)stack[sp], stack[sp]);
    } break;
    case PRINT: {
      int *a = (int *)stack[sp--];

      printf("%d\n", *a);

      if (a > &program[num_instructions + 1] || a < &program[0]) {
        free(a);
      }
    } break;
    default:
      bail("Invalid opcode %u", op);
      break;
    }
  }

  dbg("sp -> %d", sp);
}
