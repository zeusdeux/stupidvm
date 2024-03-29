#ifndef UTIL_H_
#define UTIL_H_

#include <stdio.h>
#include <stdlib.h>

#define bail(...) {                                     \
    fprintf (stderr, "%s:%d: ", __FILE__, __LINE__);    \
    fprintf (stderr, __VA_ARGS__);                      \
    fprintf (stderr, "\n");                             \
    exit(1);                                            \
  }

#ifdef ZDX_TRACE_ENABLE
#define dbg(...) {                                                      \
    fprintf (stderr, "%s:%d:\t[%s] ", __FILE__, __LINE__, __func__);      \
    fprintf (stderr, __VA_ARGS__);                                      \
    fprintf (stderr, "\n");                                             \
  }
#else
#define dbg(...) {}
#endif // ZDX_TRACE_ENABLE

#ifdef NZDX_LOG_ENABLE
#define log(level, ...) {}
#else
typedef enum {
  L_ERROR = 1,
  L_WARN,
  L_INFO
} ZDX_LOG_LEVEL;

char *ZDX_LOG_LEVEL_STR[] = {
  "",
  "ERROR",
  "WARN",
  "INFO"
};

#define log(level, ...) {                                       \
    fprintf (stderr, "%s:%d: ", __FILE__, __LINE__);            \
    fprintf (stderr, "[%s] ", ZDX_LOG_LEVEL_STR[level]);        \
    fprintf (stderr, __VA_ARGS__);                              \
    fprintf (stderr, "\n");                                     \
  }
#endif // NZDX_LOG_ENABLE

#endif // UTIL_H_
