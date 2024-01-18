#include <stdio.h>

#include "indirect_libs_include.h"

/* #define bruh(type, ...) type potato[] = {__VA_ARGS__} */

// DEBUG
// gcc -DZDX_TRACE_ENABLE -g -Wall -pedantic -Wextra -Wdeprecated -Wno-gnu-statement-expression-from-macro-expansion vec_test.c -o ./vec_test && leaks --atExit -- ./vec_test
// RELEASE
// gcc -O3 -Wall -Wno-gnu-statement-expression-from-macro-expansion ./vec_test.c -o ./vec_test
int main(void)
{
  /* bruh(int, 1, 2, 3); */
  /* printf("%d %d %d", potato[0], potato[1], potato[2]); */

  Vec v1 = vec_create(NULL);
  vec_free(v1);

  /* Vec v2 = vec_create((char []){}); */
  /* vec_free(v2); */

  // this does 3 stack allocation for {1, 2, 3} due to how
  // vec_create macro is written unfortunately
  Vec v3 = vec_create((int []){1, 2, 3});
  int *a = (int *)v3.data;

  if (v3.valid) {
    printf("11111111\ndata[0] %d\ndata[1] %d\ndata[2] %d\ndata[3] %d\nvec length %zu\n", a[0], a[1], a[2], a[3], v3.length);
    a = vec_push(v3, 200);
    printf("22222222\ndata[0] %d\ndata[1] %d\ndata[2] %d\ndata[3] %d\nvec length %zu\n", a[0], a[1], a[2], a[3], v3.length);
    a = vec_push(v3, 100, 50, 60);
    printf("33333333\ndata[0] %d\ndata[1] %d\ndata[2] %d\ndata[3] %d\ndata[4] %d\ndata[5] %d\ndata[6] %d\nvec length %zu\n",
           a[0], a[1], a[2], a[3], a[4], a[5], a[6], v3.length);


  }

  vec_free(v3);

  return 0;
}
