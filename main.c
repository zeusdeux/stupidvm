#include <stdio.h>
#include <stdlib.h>

enum Instructions {
  INUM, // 0
  IADD, // 1
  IMUL, // 2
  ISTORE, // 3
  ILOAD, // 4
  STR, // 5
  POP, // 6
  PRINT, // 7
  PRINTLN, // 8
  HALT // 9
};

const char *ins[10] = {
  "INUM",
  "IADD",
  "IMUL",
  "ISTORE",
  "ILOAD",
  "STR",
  "POP",
  "PRINT",
  "PRINTLN",
  "HALT"
};

/*
  INUM 1
  INUM 2
  IADD
  HALT
*/

const int code[14] = {0, 1,
                      0, 2,
                      1,
                      8,
                      0, 200,
                      3, 0,
                      4, 0,
                      7,
                      9};

void run(const int *code, int startingIndex, int dataSize, int trace) {
  int data[dataSize];
  int stack[1000];
  int sp = -1;
  int ip = startingIndex;
  // int fp = -1;

  int opcode = code[ip]; // fetch

  int x, y, idx;


  while (1) {

    if (trace) {
      fprintf(stderr, "%04d: %s(%d) ", ip, ins[opcode], opcode);
    }

    switch(opcode) { // decode
    case INUM: // execute INUM instruction
      x = code[++ip];
      stack[++sp] = x;
      if (trace) fprintf(stderr, "%i", x);
      break;

    case IADD:
      y = stack[sp--];
      x = stack[sp--];
      stack[++sp] = x + y;
      break;

    case IMUL:
      y = stack[sp--];
      x = stack[sp--];
      stack[++sp] = x * y;
      break;

    case ISTORE:
      idx = code[++ip];
      data[idx] = stack[sp--];
      if (trace) fprintf(stderr, "%i", idx);
      break;

    case ILOAD:
      idx = code[++ip];
      sp++;
      stack[sp] = data[idx];
      if (trace) fprintf(stderr, "%i", idx);
      break;

    case STR:
      break;

    case POP:
      sp--;
      break;

    case PRINT:
      if (trace) fprintf(stderr, "\n");
      printf("%i", stack[sp--]);
      fflush(stdout);
      break;

    case PRINTLN:
      if (trace) fprintf(stderr, "\n");
      printf("%i\n", stack[sp--]);
      break;

    case HALT:
      if (trace) fprintf(stderr, "\n");
      //printf("Top of stack (%i) is %i\n", sp, stack[sp]);
      return;

    default: fprintf(stderr, "Dafaq");
      exit(1);
    }

    if (trace) fprintf(stderr, "\n");

    // move to next opcode
    opcode = code[++ip];

  }
}

int main(int args, char **argv) {
  run(code, 0, 0, 1);
  return 0;
}
