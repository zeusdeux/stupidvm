#ifndef VEC_H_
#define VEC_H_

// forward declaration to treat Vec as an opaque type
typedef struct Vec Vec;

void vec_create(void *arr);
void vec_free(Vec vec);
void vec_push(Vec vec, ...);
/* void vec_pop(vec_t *vec); */
/* void vec_concat(vec_t *into_vec, vec_t *v); */

/**
 * STB style single file lib.
 * To use:
 * #define VEC_IMPLEMENTATION in only on C file in your codebase
 * This will inline the implementation.
 * Rest will only get the declarations from above aka usual header file things
 */
#ifdef VEC_IMPLEMENTATION

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include "util.h"

#define MIN_CAPACITY(element_sz) 4 * (element_sz)
#define SCALE_FACTOR 2 // double the size whenever array is grown/expanded

typedef struct Vec {
  size_t length;
  size_t capacity;
  size_t element_sz;
  bool valid;
  uint8_t *data; // raw bytes of data. Caller manages casting
} Vec;


#define dbg_vec(label, res) dbg("%s"                            \
                                "\tvec.data %p"                 \
                                "\tvec.capacity (bytes) %zu"    \
                                "\tvec.element_sz (bytes) %zu"  \
                                "\tvec.length %zu"              \
                                "\tvec.valid %s",               \
                                label,                          \
                                (void *)res.data,               \
                                res.capacity,                   \
                                res.element_sz,                 \
                                res.length,                     \
                                res.valid ? "true" : "false")

#define dbg_vec_ptr(label, res) dbg("%s"                                \
                                    "\tvec.data %p"                     \
                                    "\tvec.capacity (bytes) %zu"        \
                                    "\tvec.element_sz (bytes) %zu"      \
                                    "\tvec.length %zu"                  \
                                    "\tvec.valid %s",                   \
                                    label,                              \
                                    (void *)res->data,                  \
                                    res->capacity,                      \
                                    res->element_sz,                    \
                                    res->length,                        \
                                    res->valid ? "true" : "false")

void vec_resize_(Vec *vec)
{
  dbg_vec_ptr("vec_resize_:input>", vec);

  if (vec == NULL || vec->capacity == 0) {
    // TODO: panic or fail silently or log to stderr and recover?
    return;
  }

  size_t new_capacity = vec->capacity * SCALE_FACTOR;
  void *resized_data_ptr = realloc(vec->data, new_capacity);

  if (resized_data_ptr) {
    vec->data = resized_data_ptr;
    vec->capacity = new_capacity;
  } else {
    dbg("Vec:Error> Vector resize failed. %s\n", strerror(errno));
    vec->valid = false;
  }

  dbg_vec_ptr("vec_resize_:output>", vec);
}

#define vec_create(...) vec_create_((__VA_ARGS__), (__VA_ARGS__) == NULL ? 0 : sizeof((__VA_ARGS__)), (__VA_ARGS__) == NULL ? 0 : sizeof((__VA_ARGS__[0])))
Vec vec_create_(void *arr, size_t orig_capacity, size_t element_sz)
{
  dbg("vec_create:input>"
      "\tarr ptr %p"
      "\tarr capacity (bytes) %zu"
      "\telement size (bytes) %zu",
      arr,
      orig_capacity,
      element_sz);

  size_t length;
  size_t capacity;

  element_sz = element_sz ? element_sz : 4; // default to creating an element of width 4 bytes. e.g., int
  length = arr == NULL ? 0 : orig_capacity / element_sz; // calculate length before setting a default for orig_capacity if it's zero
  orig_capacity = orig_capacity ? orig_capacity : MIN_CAPACITY(element_sz); // default to 4 elements of width element_sz

  // reserved capacity for vector is always greater than or equal to MIN_CAPACITY(element_sz)
  capacity = orig_capacity < MIN_CAPACITY(element_sz) ? MIN_CAPACITY(element_sz) : orig_capacity;

  // allocate contigous bytes, zeroed out
  void *data = calloc(capacity, 1);

  Vec res = {
    .capacity = capacity,
    .length = length,
    .element_sz = element_sz,
    .valid = data ? true : false,
    .data = data
  };


  if (!res.valid) {
    dbg_vec("vec_create:output>", res);
    return res;
  }

  if (length) {
    // byte by byte copy of input array to vec.data with extra zeroed out
    for (size_t i = 0; i < res.capacity; i++) {
      uint8_t *data_ptr = (uint8_t *)res.data + i;
      if (i >= orig_capacity) {
        /* dbg("leaving value %d at %p [index %zu] as is", *data_ptr, data_ptr, i); */
        continue;
      } else {
        uint8_t val = *((uint8_t *)arr + i);
        /* dbg("storing value %d at %p [index %zu]", val, data_ptr, i); */
        *data_ptr = val;
      }
    }
  }

  dbg_vec("vec_create:output>", res);

  return res;
}

void vec_free(Vec vec)
{
  free(vec.data);
}

/**
 * this macro uses non-standard "gnu statement expressions"
 * clang and gcc both support it
 * more here: https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html
 * It uses it to return a correctly typed pointer to vec->data.
 *
 * Example:
 *
 * Vec v = vec_create(NULL); // create empty vector (defaults to int)
 * int *arr = (int *)v.data;
 *
 * arr = vec_push(v, 200, 300, 400, 500);
 * printf("a[3] => %d, length => %zu\n", arr[3], v.length); // output: a[3] => 500, length => 4
 *
 * arr = vec_push(v, 600, 700, 800); // triggers resize and potential v.data relocation in memory
 * printf("a[6] => %d, length => %zu\n", arr[6], v.length); // output: a[6] => 800, length => 7
 *
 * vec_free(v);
 */
#define vec_push(vec, ...)                                              \
  ({                                                                    \
    Vec *__zdx_##vec = vec_push_ (&(vec), (__typeof__((__VA_ARGS__))[]){__VA_ARGS__}, sizeof((__typeof__((__VA_ARGS__))[]){__VA_ARGS__})); \
    (__typeof__((__VA_ARGS__)) *)__zdx_##vec->data;                               \
  })

Vec *vec_push_(Vec *vec, void *els, size_t reqd_capacity)
{
  dbg_vec_ptr("vec_push:input>", vec);
  dbg("vec_push:input>\telement ptr %p\trequired capacity %zu", els, reqd_capacity);

  assert(reqd_capacity % vec->element_sz == 0 && "vec_push> Type of element being pushed doesn't match vector type");

  if (vec->valid) {
    size_t last_item_offset = vec->length * vec->element_sz;
    size_t remaining_capacity = vec->capacity - last_item_offset;

    dbg("vec_push:input>\toffset of last vec item %zu\tremaining capacity in vec %zu\n", last_item_offset, remaining_capacity);


    if (reqd_capacity > remaining_capacity) {
      vec_resize_(vec);

      if (!vec->valid) {
        dbg_vec_ptr("vec_push:error> Invalid vector", vec);
        return vec;
      }
    }

    uint8_t *start_data_ptr = (uint8_t *)vec->data + last_item_offset;

    dbg("start_data_ptr %p", (void *)start_data_ptr);
    for (size_t i = 0; i < reqd_capacity; i++) {
      uint8_t *data_ptr = start_data_ptr + i;
      uint8_t val = *((uint8_t *)els + i);

      *data_ptr = val;

      dbg("data_ptr %p, val %d", (void *)data_ptr, *data_ptr);
    }

    vec->length += reqd_capacity / vec->element_sz;
  } else {
    dbg("vec_push:input>Invalid vector");
  }
  return vec;
}

#endif // VEC_IMPLEMENTATION
#endif // VEC_H_
