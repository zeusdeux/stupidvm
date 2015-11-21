#include <stdio.h>
#include <stdlib.h>
#include "vm.h"

enum Instructions {
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
  HALT      // 12
};

const char *ins[] = {
  "",
  "INUM",     // INUM <integer>                                                     : Pushes integer onto stack
  "IADD",     // IADD                                                               : pops two from stack to add and pushes result back
  "IMUL",     // IMUL                                                               : Same as IADD but multiplies the two popped values
  "ISTORE",   // ISTORE <index>                                                     : Stores an integer into data segment at given index
  "ILOAD",    // ILOAD <index>                                                      : Loads integer from data at given index
  "STR",      // STR <string length + 1> <list of integers till \0 or 000 is found> : Pushes string onto stack (strlen + 1 is to accommodate \0 as end of string marker)
  "POP",      // POP                                                                : Pops top of stack
  "IPRINT",   // IPRINT                                                             : Prints top of stack. No trailing newline added
  "IPRINTLN", // IPRINTLN                                                           : Same as PRINT but adds a trailing newline
  "SPRINT",   // SPRINT                                                             : Prints top of stack as string
  "SPRINTLN", // SPRINTLN                                                           : Prints top of stack as string with trailing newline
  "HALT"      // HALT                                                               : Stop execution
};

void run(const int *code, int startingIndex, int dataSize, int trace) {
  int data[dataSize]; // arbitrary memory area
  void *stack[1000]; // runtime stack
  int sp = -1;
  int ip = startingIndex;
  // int fp = -1; // frame pointer

  int opcode = code[ip]; // fetch

  while (1) {

    if (trace) {
      fprintf(stderr, "%04d: %s(%d) ", ip, ins[opcode], opcode);
    }

    switch(opcode) { // decode
      // execute INUM instruction
    case INUM: {
      stack[++sp] = (void *)&code[++ip];
      if (trace) fprintf(stderr, "%i @ %p", *((int *) stack[sp]), stack[sp]);
      break;
    }

    case IADD: {
      int y = *((int *)stack[sp--]);
      int x = *((int *)stack[sp--]);
      int sum = x + y;

      stack[++sp] = &sum;
      break;
    }

    case IMUL: {
      int y = *((int *)stack[sp--]);
      int x = *((int *)stack[sp--]);
      int prod = x * y;

      stack[++sp] = &prod;
      break;
    }

    case ISTORE: {
      int idx = code[++ip];

      data[idx] = *((int *)stack[sp--]);
      if (trace) fprintf(stderr, "%i", idx);
      break;
    }

    case ILOAD: {
      int idx = code[++ip];
      int val = data[idx];

      stack[++sp] = &val;
      if (trace) fprintf(stderr, "%i", idx);
      break;
    }

    case STR: {
      int lenOfStr = code[++ip];
      char *str = (char *)malloc(lenOfStr * sizeof(char));

      //printf("\nlenOfStr %d", lenOfStr);
      for (int i = 0; i < lenOfStr; i++) {
        int c = code[++ip];

        str[i] = (char)c;
      }
      stack[++sp] = str;
      if (trace) fprintf(stderr, "%s @ %p", stack[sp], stack[sp]);
    }
      break;

    case POP:
      sp--;
      break;

    case IPRINT:
      if (trace) fprintf(stderr, "\n");
      printf("%i", *((int *)stack[sp--]));
      fflush(stdout);
      break;

    case IPRINTLN:
      if (trace) fprintf(stderr, "\n");
      printf("%i\n", *((int *)stack[sp--]));
      break;

    case SPRINT:
      if (trace) fprintf(stderr, "\n");
      printf("%s", stack[sp--]);
      fflush(stdout);
      free(stack[sp + 1]);
      break;

    case SPRINTLN:
      if (trace) fprintf(stderr, "printing string @ %p\n", stack[sp]);
      printf("%s\n", stack[sp--]);
      free(stack[sp + 1]);
      break;

    case HALT:
      if (trace) fprintf(stderr, "\n");
      if (sp != -1) {
        fprintf(stderr, "Stack still contains things. You are leaking memory.\n");
        fprintf(stderr, "Size of stack is %d and top of stack (%i) is %p\n", sp + 1, sp, stack[sp]);
      }

      return;

    default: fprintf(stderr, "Dafaq %s(%d)", ins[opcode], opcode);
      exit(1);
    }

    if (trace) fprintf(stderr, "\n");

    // move to next opcode
    opcode = code[++ip];

  }
}
