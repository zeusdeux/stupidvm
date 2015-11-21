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
  "JMP",
  "JMPF",     // 13
  "JMPT",     // 14
  "JMPLT",    // 15
  "JMPLTE",   // 16
  "JMPGT",    // 17
  "JMPGTE",   // 18
  "JMPEQ",    // 19
  "JMPNE",
  "HALT"      // HALT                                                               : Stop execution
};

int bail(const char *error) {
  fprintf(stderr, "%s\n", error);
  exit(1);
}

void run(const int *code, int startingAddr, int trace) {

  if (code[0] != STUPIDVMMARKER) bail(EINVALIDBC);

  int noOfBytecodes   = code[1];
  int dataSegmentSize = code[2];

  int data[dataSegmentSize];   // arbitrary memory area
  void *stack[MAX_STACK_SIZE]; // runtime stack

  int sp              = -1;
  int ip              = startingAddr;
  // int fp           = -1; // frame pointer


  if (trace) fprintf(stderr, "No., of bytecodes: %d\n\n", noOfBytecodes);

  while (ip < noOfBytecodes) {
    int opcode = code[ip++]; // fetch

    if (trace) {
      fprintf(stderr, "%04d: %s(%d) ", ip, ins[opcode], opcode);
    }

    switch(opcode) { // decode opcode & execute
    case INUM: {
      stack[++sp] = (void *)&code[ip++];
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
      int idx = code[ip++];

      data[idx] = *((int *)stack[sp--]);
      if (trace) fprintf(stderr, "%i", idx);
      break;
    }

    case ILOAD: {
      int idx = code[ip++];
      int val = data[idx];

      stack[++sp] = &val;
      if (trace) fprintf(stderr, "%i", idx);
      break;
    }

    case STR: {
      int lenOfStr = code[ip++];
      char *str    = (char *)malloc(lenOfStr * sizeof(char));

      for (int i = 0; i < lenOfStr; i++) {
        int c = code[ip++];

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

    case JMP: {
      int jumpAddr = code[ip++];

      if (trace) fprintf(stderr, "%d", jumpAddr);

      // check if addr to jmp to is within the bytecodes given
      if (jumpAddr >= noOfBytecodes) bail(EINVALIDJMP);
      ip = jumpAddr;
      break;
    }

    case JMPF: {
      int jumpAddr = code[ip++];

      if (trace) fprintf(stderr, "%d", jumpAddr);

      if (*((int *) stack[sp]) == FALSE) {
        if (jumpAddr >= noOfBytecodes) bail(EINVALIDJMP);
        ip = jumpAddr;
      }
      break;
    }

    case JMPT: {
      int jumpAddr = code[ip++];

      if (trace) fprintf(stderr, "%d", jumpAddr);

      if (*((int *) stack[sp]) == TRUE) {
        if (jumpAddr >= noOfBytecodes) bail(EINVALIDJMP);
        ip = jumpAddr;
      }
      break;
    }

    case JMPLT: { // jump if value on stack is lesser than the one given to this instruction
      int val = *((int *)stack[sp]);
      int arg = code[ip++];
      int jumpAddr = code[ip++];

      if (trace) fprintf(stderr, "%d %d", arg, jumpAddr);

      if (val < arg) {
        if (jumpAddr >= noOfBytecodes) bail(EINVALIDJMP);
        ip = jumpAddr;
      }
      break;
    }

    case JMPLTE: {
      int val = *((int *)stack[sp]);
      int arg = code[ip++];
      int jumpAddr = code[ip++];

      if (trace) fprintf(stderr, "%d %d", arg, jumpAddr);

      if (val <= arg) {
        if (jumpAddr >= noOfBytecodes) bail(EINVALIDJMP);
        ip = jumpAddr;
      }
      break;
    }

    case JMPGT: {
      int val = *((int *)stack[sp]);
      int arg = code[ip++];
      int jumpAddr = code[ip++];

      if (trace) fprintf(stderr, "%d %d", arg, jumpAddr);

      if (val > arg) {
        if (jumpAddr >= noOfBytecodes) bail(EINVALIDJMP);
        ip = jumpAddr;
      }
      break;
    }

    case JMPGTE: {
      int val = *((int *)stack[sp]);
      int arg = code[ip++];
      int jumpAddr = code[ip++];

      if (trace) fprintf(stderr, "%d %d", arg, jumpAddr);

      if (val >= arg) {
        if (jumpAddr >= noOfBytecodes) bail(EINVALIDJMP);
        ip = jumpAddr;
      }
      break;
    }

    case JMPEQ: {
      int val = *((int *)stack[sp]);
      int arg = code[ip++];
      int jumpAddr = code[ip++];

      if (trace) fprintf(stderr, "%d %d", arg, jumpAddr);

      if (val == arg) {
        if (jumpAddr >= noOfBytecodes) bail(EINVALIDJMP);
        ip = jumpAddr;
      }
      break;
    }

    case JMPNE: {
      int val = *((int *)stack[sp]);
      int arg = code[ip++];
      int jumpAddr = code[ip++];

      if (trace) fprintf(stderr, "%d %d", arg, jumpAddr);

      if (val != arg) {
        if (jumpAddr >= noOfBytecodes) bail(EINVALIDJMP);
        ip = jumpAddr;
      }
      break;
    }

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
  }
}
