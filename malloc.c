//
// Created by Thomas Povinelli on 5/18/22.
//

#include "malloc.h"

#include "errno.h"
#include "tpsupport.h"

void* memset(void* pointer, int value, size_t size)
{
  char* p = pointer;
  for (size_t i = 0; i < size; i++)
    p[i] = value;
  return pointer;
}

void* memcpy(void* destination, void const* src, size_t size)
{
  for (size_t i = 0; i < size; i++)
    ((char*) destination)[i] = ((char*) src)[i];
  return destination;
}

struct free_block* split_block(struct free_block* block,
                               unsigned int       cur_order,
                               unsigned int       target_order)
{
  unsigned int current_order = cur_order;
  void*        start         = block;

  while (current_order > target_order) {
    struct free_block* half = (start + (1 << (current_order - 1)));
    UNSETALLOC(half);
    SETORDER(half, current_order - 1);
    relink_free_block(half);
    current_order--;
  }
  return start;
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
    return pointer;
  }
}

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
      assert(errno == ENOMEM);
      return NULL;
    }

    SETALLOC(candidate);
    return candidate + FREE_OFF_BLOCK;
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
