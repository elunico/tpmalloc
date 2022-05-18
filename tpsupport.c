//
// Created by Thomas Povinelli on 5/18/22.
//
#include "tpsupport.h"

struct free_block free_lists[LISTCOUNT];

long tp_lclz(long v)
{
  unsigned long c  = 1UL << ((sizeof(v) * 8) - 1);
  long          lz = 0;
  while ((c & v) == 0) {
    c >>= 1;
    lz++;
  }
  return lz;
}

void abort()
{
#if defined(__has_builtin)
#if __has_builtin(__builtin_traps)
  __builtin_trap();
#else
  __asm__ volatile("movl $0x0, %%eax\n"
                   "movl $0x0, %%ebx\n"
                   "int     $134" ::
                       : "eax", "ebx");
#endif
#if defined(__i386) || defined(__x86) || defined(__x86_64__)
  __asm__ volatile("movl $0x0, %%eax\n"
                   "movl $0x0, %%ebx\n"
                   "int     $134" ::
                       : "eax", "ebx");
#else
#error "unsupported platform"
#endif
#endif
}

struct free_block* ptr_to_block(void* pointer)
{
  struct free_block* block = pointer - sizeof(struct free_block);
  assert(ISALLOC(block));
  assert(ISMAGIC(block));
  UNSETALLOC(block);
  return block;
}

void* req_new_block(size_t order)
{
  void*              candidate = sbrk(1 << order);
  struct free_block* block     = candidate;
  block->order                 = order;
  SETALLOC(block);
  SETMAGIC(block);
  SETNOTSENT(block);
  return candidate;
}

void* unlink_free_block(size_t order)
{
  void* candidate        = free_lists[order].next;
  free_lists[order].next = free_lists[order].next->next;
  if (free_lists[order].next != NULL) {
    free_lists[order].next->prev = &free_lists[order];
  }
  SETALLOC(candidate);
  return candidate;
}

void relink_free_block(struct free_block* block)
{
  int order              = block->order;
  block->prev            = &free_lists[order];
  block->next            = free_lists[order].next;
  free_lists[order].next = block;
}
