//
// Created by Thomas Povinelli on 5/18/22.
//

#include "malloc.h"


void* malloc(size_t size)
{
  if (size == 0) {
    return NULL;
  }

  size_t rlsize = REAL_SIZE(size);
  size_t order  = ORDER_FROM_RLSIZE(rlsize);

  if (free_lists[order].next != NULL) {
    void* candidate = unlink_free_block(order);
    return candidate + sizeof(struct free_block);
  } else {
    void* candidate = req_new_block(order);
    return candidate + sizeof(struct free_block);
  }
}

void free(void* pointer)
{
  if (pointer == NULL) {
    write(STDOUT_FILENO, "free() called on NULL!\n", 25);
    abort();
  }
  struct free_block* block = ptr_to_block(pointer);
  relink_free_block(block);
}