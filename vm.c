#include <stdio.h>
#include <stdlib.h>
#include "vm.h"


// TOS = top of stack
const char *ins[] = {
  "",
  "INUM",     // INUM <integer>                                             : Pushes integer onto stack
  "IADD",     // IADD                                                       : pops two from stack to add and pushes result back
  "IMUL",     // IMUL                                                       : Same as IADD but multiplies the two popped values
  "ISTORE",   // ISTORE <index>                                             : Stores an integer into data segment at given index
  "ILOAD",    // ILOAD <index>                                              : Loads integer from data at given index
  "STR",      // STR <str len + 1> <list of chars as ints + 0 (dec for \0)> : Pushes string onto stack (strlen + 1 is to accommodate \0 as end of string marker)
  "POP",      // POP                                                        : Pops top of stack
  "IPRINT",   // IPRINT                                                     : Prints top of stack. No trailing newline added
  "IPRINTLN", // IPRINTLN                                                   : Same as PRINT but adds a trailing newline
  "SPRINT",   // SPRINT                                                     : Prints top of stack as string
  "SPRINTLN", // SPRINTLN                                                   : Prints top of stack as string with trailing newline
  "JMP",      // JMP addr                                                   : Jump to instruction at address addr
  "JMPF",     // JMPF addr                                                  : Jump to instruction at addr if TOS is FALSE. Doesn't consume TOS.
  "JMPT",     // JMPT addr                                                  : Jump to instruction at addr if TOS is TRUE. Doesn't consume TOS.
  "JMPLT",    // JMPLT val addr                                             : Jump to instruction at addr if TOS is lesser than val. Doesn't consume TOS.
  "JMPLTE",   // JMPLTE val addr                                            : Jump to instruction at addr if TOS is lesser than or equal to val. Doesn't consume TOS.
  "JMPGT",    // JMPGT val addr                                             : Jump to instruction at addr if TOS is greater than val. Doesn't consume TOS.
  "JMPGTE",   // JMPGTE val addr                                            : Jump to instruction at addr if TOS is greater than or equal to val. Doesn't consume TOS.
  "JMPEQ",    // JMPEQ val addr                                             : Jump to instruction at addr if TOS is equal to val. Doesn't consume TOS.
  "JMPNE",    // JMPNE val addr                                             : Jump to instruction at addr if TOS is not equal to val. Doesn't consume TOS.
  "HALT"      // HALT                                                       : Stop execution
};

void bail(const char *error) {
  fprintf(stderr, "%s\n", error);
  exit(1);
}

void run(const int *code, int startingAddr, int trace) {

  if (code[0] != STUPIDVMMARKER) bail(EINVALIDBC);

  int noOfBytecodes   = code[1];
  int dataSegmentSize = code[2];

  int data[dataSegmentSize];   // arbitrary memory area
  void *stack[MAX_STACK_SIZE]; // runtime stack
  //  RStack rstack[MAX_STACK_SIZE];

  int sp              = -1;
  int ip              = startingAddr;
  // int fp           = -1; // frame pointer


  if (trace) {
    fprintf(stderr, "No., of bytecodes: %d\n\n", noOfBytecodes);
    fprintf(stderr, "ADDR: OPCODE\tARGS\n--------------------\n");
  }

  while (ip < noOfBytecodes) {
    int opcode = code[ip++]; // fetch

    if (trace) {
      fprintf(stderr, "%04d: %s\t", ip - 1, ins[opcode]);
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
