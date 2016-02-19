#include "vm.h"

/*

Bytecode format:
----------------

first 4 bytes = STUPIDVMMARKER
next 4 bytes  = total number bytecodes + 3 (for metadata stored in the first 12 bytes)
next 4 bytes  = size of data segment

From there onwards it's the actual bytecodes.

*/

const int code[] = {STUPIDVMMARKER,                                                 // AWESOME BYTECODE MARKER
                    44,                                                             // No of bytecodes i.e., length of this array (includes the meta data at indices 0, 1 and 2)
                    2,                                                              // size of data segment. It should be, atleast as big as the largest value given to a <type>STORE command
                    1, 2,                                                           // INUM 2
                    1, 3,                                                           // INUM 3
                    2,                                                              // IADD
                    9,                                                              // IPRINTLN
                    1, 10,                                                          // INUM 10
                    1, 4,                                                           // INUM 4
                    2,                                                              // IADD
                    9,                                                              // IPRINTLN
                    1, 200,                                                         // INUM 200
                    4, 2,                                                           // ISTORE 2
                    5, 2,                                                           // ILOAD 2
                    9,                                                              // IPRINTLN
                    6, 12, 104, 101, 108, 108, 111, 32, 119, 111, 114, 108, 100, 0, // STR 12 hello world\0
                    6, 4, 65, 66, 67, 0,                                            // STR 4 ABC\0
                    11,                                                             // SPRINTLN
                    21};                                                            // HALT

// branching test
const int code2[] = {STUPIDVMMARKER,
                     22,
                     0,
                     INUM, 2,
                     INUM, 3,
                     IADD,
                     JMPLTE, 10, 5,
                     IPRINTLN,
                     STR, 6, 68, 79, 78, 69, 33, 0,
                     SPRINTLN,
                     HALT};

int main(int args, char **argv) {
  VM vm = initVM(code2, 3, argv[1] || 0); // pass 1 as first arg to see disassembly
  run(&vm);
  return 0;
}
