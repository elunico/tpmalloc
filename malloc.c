//
// Created by Thomas Povinelli on 5/18/22.
//

#include "malloc.h"
#include "errno.h"
#include "tpsupport.h"

void* calloc(size_t size, size_t count)
{
  if (size == 0 || count == 0)
    return NULL;

  void* pointer = malloc(size * count);
  if (pointer == NULL)
    return NULL;

  memset(pointer, 0, size * count);
  return pointer;
}

void free(void* pointer)
{
  if (pointer == NULL) {
#ifdef TPMALLOC_DIE_ON_NULL_FREE
    write(STDOUT_FILENO, "free() called on NULL!\n", 25);
    abort();
#else
    return;
#endif
  }
  struct free_block* block = ptr_to_block(pointer);
  relink_free_block(block);
}

void* malloc(size_t size)
{
  if (size == 0) {
    return NULL;
  }

  if (size > (1L << (LISTCOUNT - 1))) {
    errno = ENOMEM;
    return NULL;
  }

  size_t rlsize = REAL_SIZE(size);
  size_t order  = ORDER_FROM_RLSIZE(rlsize);

  size_t free_order = order;
  while (free_lists[free_order].next == NULL && free_order < LISTCOUNT) {
    free_order++;
  }

  if (free_order == order) {
    void* candidate = unlink_free_block(order);
    SETALLOC(candidate);
    return candidate + FREE_OFF_BLOCK;
  } else if (free_order > order && free_order < LISTCOUNT) {
    struct free_block* block = unlink_free_block(free_order);
    struct free_block* ptr   = split_block(block, free_order, order);
    SETALLOC(ptr);
    return ptr + FREE_OFF_BLOCK;
  } else {
    void* candidate = req_new_block(order);

    if (candidate == NULL) {
      ASSERT(errno == ENOMEM);
      return NULL;
    }

    SETALLOC(candidate);
    return candidate + FREE_OFF_BLOCK;
  }
}

void* realloc(void* pointer, size_t new_size)
{
  if (pointer == NULL) {
    return malloc(new_size);
  }

  if (new_size == 0) {
    free(pointer);
    return NULL;
  }

  struct free_block* block  = (struct free_block*) (pointer - FREE_OFF_BLOCK);
  unsigned int       order  = GETORDER(block);
  unsigned int       rlsize = REAL_SIZE(new_size);
  unsigned int       new_order = ORDER_FROM_RLSIZE(rlsize);

  if (new_order > order) {
    void* newer = malloc(new_size);
    if (newer == NULL) {
      return NULL;
    }
    memcpy(newer, pointer, 1 << order);
    free(pointer);
    return newer;
  } else if (new_order < order) {
    void* start = split_block((struct free_block*) (pointer - FREE_OFF_BLOCK),
                              order, new_order);
    SETORDER(start, new_order);
    return pointer;
  } else {
    // TODO: if the size reduces by as little as 64 or as much as (half the
    // current size - 1) nothing happens. Theoretically I could free smaller
    // blocks if we go from for example 1000 to 600, I could free a few
    // blocks-some as large as 256 bytes, however the only time realloc "frees"
    // memory is if the new size is less than or equal to half the nearest power
    // of two of the current size when rounding up. So a block of size 1000
    // needs to be realloced to size <= 512 for any change in memory to happen
    return pointer;
  }
}
