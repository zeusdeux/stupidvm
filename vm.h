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

#ifndef EINVALIDTYPE
#define EINVALIDTYPE "Invalid data type"
#endif

#ifndef EINVALIDOPONSTR
#define EINVALIDOPONSTR "Invalid operation on string"
#endif

#ifndef EINVALIDJMP
#define EINVALIDJMP "Invalid jump operation"
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

enum Type {
  INT = 1,
  FLOAT,
  STRING
};

enum Test {
  UNCOND = 1, // unconditional jump
  EQ,
  NEQ,
  LT,
  LTE,
  GT,
  GTE
};

typedef struct DataType {
  enum Type t;
  union {
    int i;
    float f;
    char *str;
    // add custom types like classes etc here
    // they can be other structs (for composite
    // types like classes, etc)
  };
};



// new runtime stack structure
/* typedef struct Instruction { */
/*   int opcode; */
/*   DataType val; */
/* } Instruction; */

typedef struct VM {
  DataType stack[MAX_STACK_SIZE];
  const int noOfBytecodes;
  const int *code;
  const int trace;
  DataType *data;
  int ip; // instruction pointer
  int sp; // stack pointer
  int fp; // frame pointer
} VM;

extern const char *ins[];

VM initVM(const int *code, const int startingAddr, const int trace);
void run(VM *vm);

int getCurrentByteCodeAndIncIP(VM *vm);

void push(VM *vm, DataType val);
DataType pop(VM *vm);

void add(VM *vm, enum Type t);
void mult(VM *vm, enum Type t);
void subt(VM *vm, enum Type t);

void store(VM *vm, int idx);
void load(VM *vm, int idx);

void compare(VM *vm, enum Type t); // type dictates comparison algo

// type of test decides jump or not
// type of value tells type of result pushed by COMPARE onto stack
void jump(VM *vm, enum Test te, int newIP, enum Type t);

#endif
