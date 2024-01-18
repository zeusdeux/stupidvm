#include <stdio.h>
#include <stdbool.h>
#include "vm.h"

// With tracing
// gcc -DZDX_TRACE_ENABLE -std=c17 -Wall -pedantic -Wdeprecated -Wextra vm.c main.c -o ./vm
// Without tracing
// gcc -std=c17 -Wall -pedantic -Wdeprecated -Wextra vm.c main.c -o ./vm
int main(void)
{
  const int program[] = {
    12,
    PUSH, 10,
    PUSH, 20,
    IADD, // 30
    PUSH, 40,
    PUSH, 50,
    IADD, // 90
    IADD, // 120
    PRINT
  };

  run(program);

  return 0;
}


// []
