#include <stdio.h>
#include "./indirect_libs_include.h"

typedef struct {
  char *input, *output;
} ReplHistoryItem;

typedef struct {
  bool valid;
  size_t length;
  size_t capacity;
  ReplHistoryItem *items;
  int i;
} ReplHistory;

// Release:
// gcc -O3 -Wall ./dyn_array_test.c -o ./dyn_array_test && leaks --atExit -- ./dyn_array_test
// Debug:
// gcc -DZDX_TRACE_ENABLE -g -Wall -Wextra -Wdeprecated -pedantic ./dyn_array_test.c -o ./dyn_array_test && leaks --atExit -- ./dyn_array_test
int main(void)
{
  ReplHistory replHistory = {0};

  da_push(&replHistory,
          (ReplHistoryItem){ .input = "3 + 4", .output = "7" },
          (ReplHistoryItem){ .input = "console.log(\"omg\")", .output = "omg" },
          (ReplHistoryItem){ .input = "sin(90)", .output = "1" },
          (ReplHistoryItem){ .input = "typeof []", .output = "array" }
          );

  printf("input %s output %s\n", replHistory.items[0].input, replHistory.items[0].output);
  printf("input %s output %s\n", replHistory.items[1].input, replHistory.items[1].output);
  printf("input %s output %s\n", replHistory.items[2].input, replHistory.items[2].output);
  printf("input %s output %s\n", replHistory.items[3].input, replHistory.items[3].output);

  printf("ARRAY LENGTH %zu\n", replHistory.length);

  replHistory.i = 200;

  da_push(&replHistory, (ReplHistoryItem){ .input = "unknownFn(12)", .output = "Error: Unknown function `unknownFn`" });

  printf("input %s output %s\n", replHistory.items[4].input, replHistory.items[4].output);

  printf("ARRAY LENGTH %zu\n", replHistory.length);

  ReplHistoryItem popped = da_pop(&replHistory);
  da_pop(&replHistory);
  da_pop(&replHistory);
  da_pop(&replHistory);

  popped = da_pop(&replHistory);
  printf("input %s output %s\n", popped.input, popped.output);

  printf("ARRAY LENGTH %zu\n", replHistory.length);

  printf("replHistory.i %d\n", replHistory.i);

  replHistory.i += 900;

  printf("replHistory.i %d\n", replHistory.i);

  da_free(&replHistory);

  return 0;
}

/* int main(void) */
/* { */
/*   ReplHistory r = {0}; */

/*   da_push(&r, (ReplHistoryItem) { .input = "3 + 4", .output = "7" }); */
/*   da_push(&r, (ReplHistoryItem) { .input = "sin(90)", .output = "1" }); */

/*   printf("input %s output %s\n", r.items[0].input, r.items[0].output); */
/*   printf("input %s output %s\n", r.items[1].input, r.items[1].output); */
/*   da_free(&r); */
/*   return 0; */
/* } */
