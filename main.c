#include "vm.h"

const int code[] = {1, 2,                                                           // INUM 2
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
                    6, 4, 65, 66, 67, 0,
                    11,                                                             // SPRINTLN
                    12};

int main(int args, char **argv) {
  run(code, 0, 10, argv[1] || 0); // pass 1 as first arg to see disassembly
  return 0;
}
