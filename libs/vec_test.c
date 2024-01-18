#include <stdio.h>

#define VEC_IMPLEMENTATION
#include "vec.h"

/* #define test(...) broski((__VA_ARGS__), (__VA_ARGS__) == NULL ? 0 : "omg") */

// gcc -DZDX_TRACE_ENABLE -g -Wall -pedantic -Wextra -Wdeprecated vec_test.c -o ./vec_test && leaks --atExit -- ./vec_test
int main(void)
{
  /* test(NULL); */
  /* test({1, 2, 3}); */
  /* test((int []){1, 2, 3}); */


  Vec v1 = vec_create(NULL);
  Vec v2 = vec_create((char []){0});

  // this does 3 stack allocation for {1, 2, 3} due to how
  // vec_create macro is written unfortunately
  Vec v3 = vec_create((int []){1, 2, 3});
  int *a = (int *)v3.data;

  if (v3.valid) {
    printf("11111111\ndata[0] %d\ndata[1] %d\ndata[2] %d\ndata[3] %d\nvec length %zu\n", a[0], a[1], a[2], a[3], v3.length);
    vec_push(&v3, 200);
    printf("22222222\ndata[0] %d\ndata[1] %d\ndata[2] %d\ndata[3] %d\nvec length %zu\n", a[0], a[1], a[2], a[3], v3.length);
    vec_push(&v3, 100, 900);
    printf("33333333\ndata[0] %d\ndata[1] %d\ndata[2] %d\ndata[3] %d\ndata[4] %d\ndata[5] %d\nvec length %zu\n",
           a[0], a[1], a[2], a[3], a[4], a[5], v3.length);

  }

  vec_free(v1);
  vec_free(v2);
  vec_free(v3);

  return 0;
}
