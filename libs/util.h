#ifndef UTIL_H_
#define UTIL_H_

#include <stdio.h>
#include <stdlib.h>

#define bail(...) {                                       \
    fprintf (stderr, "%s:%d: ", __FILE__, __LINE__);      \
    fprintf (stderr, __VA_ARGS__);                        \
    fprintf (stderr, "\n");                               \
    exit(1);                                              \
  }

#ifdef ZDX_TRACE_ENABLE
#define dbg(...) {                                        \
    fprintf (stderr, "%s:%d: ", __FILE__, __LINE__);      \
    fprintf (stderr, __VA_ARGS__);                        \
    fprintf (stderr, "\n");                               \
  }
#else
#define dbg(...) {}
#endif

#endif // UTIL_H_
