#ifndef DYN_ARRAY_H_
#define DYN_ARRAY_H_

#include <stdbool.h>
#include <stdint.h>

void da_free(void *const container);

#define da_push(da, ...) da_push_((da),                                 \
                                  (__typeof__((__VA_ARGS__))[]){__VA_ARGS__}, \
                                  sizeof((__typeof__((__VA_ARGS__))[]){__VA_ARGS__}), \
                                  sizeof((__VA_ARGS__)))

// Guarding against calling da_pop on an empty array is on the consumer of this library
#define da_pop(da) ({                                                   \
      if (!(da)->length) {                                              \
        log(L_ERROR, "da_pop called on empty container. Return value is undefined behaviour and may cause failures"); \
      }                                                                 \
      (da)->items[--((da)->length)];                                    \
    })
#endif // DYN_ARRAY_H_

#ifdef DYN_ARRAY_IMPLEMENTATION

#include <stdlib.h>
#include "./util.h"

#define DA_MIN_CAPACITY(element_sz) (4 * (element_sz)) // 4 elements is the min capacity
#define DA_RESIZE_FACTOR 2 // double the dyn array capacity upon resize
#define dbg_da(label, da) dbg("%s valid %d\t\t\t| length %zu\t\t\t| capacity (bytes) %zu\t\t| items %p", \
                              label, da->valid, da->length, da->capacity, (void *)da->items)

typedef struct {
  bool valid;
  size_t length;
  size_t capacity;
  uint8_t *items;
} da_contract_t_;

bool da_is_valid_(const void *const container)
{
  da_contract_t_ *da = (da_contract_t_ *)container;

  dbg("input:\t container %p", container);
  dbg_da("input:\t", da);

  if (container == NULL) {
    log(L_ERROR, "Cannot operate on NULL container");
    return 0;
  }

  bool yet_to_be_init = !da->valid && !da->capacity && !da->items && !da->length;
  bool is_valid = yet_to_be_init || (da->valid && da->capacity && da->items);

  dbg("output:\t %d", is_valid);
  return is_valid;
}

void da_resize_(void *const container, const size_t element_sz)
{
  dbg("input:\t container %p\t| element_sz %zu", container, element_sz);

  da_contract_t_ *da = (da_contract_t_ *)container;

  if (!da_is_valid_(container)) {
    da->valid = false;
    log(L_ERROR, "Cannot resize NULL container");
    return;
  }

  dbg_da("input:\t", da);

  size_t new_capacity = da->capacity ? da->capacity * DA_RESIZE_FACTOR : DA_MIN_CAPACITY(element_sz);
  void *items = da->items ? realloc(da->items, new_capacity) : calloc(new_capacity, 1);

  if (!items) {
    da->valid = false;
    log(L_ERROR, "Allocation for %zu bytes failed", new_capacity);
    return;
  }

  da->valid = true;
  da->capacity = new_capacity;
  da->items = items;

  dbg_da("output:\t", da);
}

// this function assumes element_sz is the same across ALL invocations that use the same `container`
// which is why it doesn't store element_sz in da_contract_t_ type and instead uses whatever the value
// is of element_sz on each call
void da_push_(void *const container, const void *const els, const size_t reqd_capacity, const size_t element_sz)
{
  dbg("input: container %p\t| elements %p\t| reqd_capacity (bytes) %zu\t| element_sz (bytes) %zu", container, els, reqd_capacity, element_sz);

  da_contract_t_ *da = (da_contract_t_ *)container;

  if (!da_is_valid_(container)) {
    da->valid = false;
    log(L_ERROR, "Cannot push to NULL container");
    return;
  }

  dbg_da("input:", da);

  const size_t free_capacity = da->capacity - (da->length * element_sz);

  if (reqd_capacity > free_capacity) {
    da_resize_(container, element_sz);
  }

  if (da_is_valid_(container)) {
    dbg("---- START PUSH ----");
    // byte by byte copy of els into da->items from the correct offset
    const size_t last_item_offset = da->length * element_sz;
    uint8_t *start = da->items + last_item_offset;

    dbg("last item offset %zu", last_item_offset);

    for (size_t i = 0; i < reqd_capacity; i++) {
      uint8_t *data = start + i;
      uint8_t val = *(((uint8_t *)els) + i);

      *data = val;

      /* dbg("data %p data[%zu]\t=> %#x", (void *)data, i, *data); */
    }
    dbg("----  END PUSH  ----");
    da->valid = true;
    da->length += reqd_capacity / element_sz;
    /* da->capacity -= reqd_capacity; */

    dbg_da("output:", da);
  } else {
    log(L_ERROR, "Cannot push to invalid container");
    return;
  }
}

void da_free(void *const container)
{
  da_contract_t_ *da = (da_contract_t_ *)container;

  dbg("input:\t container %p", container);
  dbg_da("input:\t", da);

  free(da->items);
  da->valid = false;
  da->length = 0;
  da->capacity = 0;
  da->items = NULL;

  dbg_da("output:\t", da);
}

#endif // DYN_ARRAY_IMPLEMENTATION
