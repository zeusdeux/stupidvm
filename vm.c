#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

VM *initVM(const int *code, const int startingAddr, const int trace) {
  VM vm; // = malloc(sizeof(VM));
  const int noOfBytecodes   = code[1];
  const int dataSegmentSize = code[2];

  if (code[0] != STUPIDVMMARKER) bail(EINVALIDBC);

  if (trace) {
    fprintf(stderr, "No., of bytecodes: %d\n\n", noOfBytecodes);
    fprintf(stderr, "ADDR: OPCODE\tARGS\n--------------------\n");
  }

  vm.sp = -1;
  vm.fp = -1;
  vm.ip = startingAddr;

  vm.trace = trace;
  vm.noOfBytecodes = noOfBytecodes;

  vm.code = code;
  vm.data = malloc(sizeof(DataType)*dataSegmentSize);

  return &vm;
}

int getCurrentByteCodeAndIncIP(VM *vm) {
  int bc = vm->code[vm->ip];

  vm->ip = vm->ip + 1;
  return bc;
}

void push(VM *vm, DataType val) {
  DataType val;

  // increment stack pointer to point
  // to next empty slot
  vm->sp = vm->sp + 1;

  // ip already incremented by getCurrentOpCode
  // hence the post increment
  // increment so that ip now points to next
  // bytecode
  vm->stack[vm->sp] = val;
}


DataType pop(VM *vm) {
  return vm->stack[vm->sp--];
}

void add(VM *vm, enum Type t) {
  DataType y = pop(vm);
  DataType x = pop(vm);
  DataType sum;

  sum.t = t;

  switch(t) {
  case INT: sum.i = x.i + y.i;
    break;
  case FLOAT: sum.f = x.f + y.f;
    break;
  case STRING: strcat(x.str, y.str);
    sum.str = x.str;
    break;
  default: bail(EINVALIDTYPE);
    break;
  }
  push(vm, sum);
}

void mult(VM *vm, enum Type t) {
  DataType y = pop(vm);
  DataType x = pop(vm);
  DataType product;

  product.t = t;

  switch(t) {
  case INT: product.i = x.i * y.i;
    break;
  case FLOAT: product.f = x.f * y.f;
    break;
  case STRING: bail(EINVALIDOPONSTR);
    break;
  default: bail(EINVALIDTYPE);
    break;
  }
  push(vm, product);
}

void subt(VM *vm, enum Type t) {
  DataType y = pop(vm);
  DataType x = pop(vm);
  DataType difference;

  difference.t = t;

  switch(t) {
  case INT: difference.i = x.i - y.i;
    break;
  case FLOAT: difference.f = x.f - y.f;
    break;
  case STRING: bail(EINVALIDOPONSTR);
    break;
  default: bail(EINVALIDTYPE);
    break;
  }
  push(vm, difference);
}

void store(VM *vm, int idx) {
  vm->data[idx] = pop(vm);
}

void load(VM *vm, int idx) {
  push(vm, vm->data[idx]);
}

// type dictates comparison algo
void compare(VM *vm, enum Type t) {
  switch(t) {
  case INT:
  case FLOAT: subt(vm, t);
    break;
  case STRING:
    DataType a = pop(vm);
    DataType b = pop(vm);
    DataType c;

    c.t = INT;
    c.i = strcmp(a.str, b,str);
    push(vm, c);
    break;
  default: bail(EINVALIDTYPE);
    break;
  }
}

// type of test decides jump or not
void jump(VM *vm, int newIP, enum Test te, enum Type t) {
  DataType topOfStack = pop(vm);
  int cmpVal;

  switch(t) {
  case INT:
  case STRING:
    cmpVal = topOfStack.i;
    break;
  case FLOAT:
    cmpVal = topOfStack.f < 0 ? -1 : (topOfStack.f == 0 ? 0 : 1);
    break;
  default: bail(EINVALIDTYPE);
  }


}


void run(VM *vm) {
  while (vm->ip < vm->noOfBytecodes) {
    int opcode = getCurrentByteCodeAndIncIP(&vm);
    DataType x;
    int idx, strLength;

    if (vm->trace) {
      fprintf(stderr, "%04d: %s\t", vm->ip - 1, ins[opcode]);
    }

    // decode & execute
    switch(opcode) {
    case INUM:
      x.t = INT;
      x.i = getCurrentByteCodeAndIncIP(&vm);
      push(&vm, x);
      break;
    case IADD: add(&vm, INT);
      break;
    case IMUL: mult(&vm, INT);
      break;
    case ISTORE:
      idx = getCurrentByteCodeAndIncIP(&vm);
      store(&vm, idx);
      break;
    case ILOAD:
      idx = getCurrentByteCodeAndIncIP(&vm);
      load(&vm, idx);
      break;
    case STR:
      strLength = getCurrentByteCodeAndIncIP(&vm);
      x.t = STRING;
      x.str = malloc(lenOfStr * sizeof(char));

      // copy char by char
      // XXX: Look into just copy strLength bytes
      // onto x.str using mmap or some cpy function
      for (i = 0; i < strLength; i++) x.str[i] = (char)getCurrentByteCodeAndIncIP(&vm);
      push(&vm, x);
      break;
    case POP:
      pop(&vm);
      break;

    }
  }
}

void run1(const int *code, int startingAddr, int trace) {

  if (code[0] != STUPIDVMMARKER) bail(EINVALIDBC);

  int noOfBytecodes   = code[1];
  int dataSegmentSize = code[2];

  int data[dataSegmentSize];   // arbitrary memory area
  void *stack[MAX_STACK_SIZE]; // runtime stack
  //  Op *ops[MAX_STACK_SIZE]; // read bytecode and stick it in there

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
