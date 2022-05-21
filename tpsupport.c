//
// Created by Thomas Povinelli on 5/18/22.
//
#include "tpsupport.h"

size_t strlen(char const* s)
{
  size_t c = 0;
  while (*s++)
    c++;
  return c;
}

void fnputsd(int fno, char const* str)
{
  size_t len = strlen(str);
  write(fno, str, len);
}

void putsd(char const* s)
{
  size_t l = strlen(s);
  write(STDOUT_FILENO, s, l);
}

void puti(int i)
{
  char c[64];
  int  offset = 63;
  if (i == 0)
    c[offset--] = '0';
  while (i > 0) {
    c[offset] = '0' + (i % 10);
    i /= 10;
    offset--;
  }
  write(STDOUT_FILENO, &c[offset + 1], 63 - offset);
}

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
#if __has_builtin(__builtin_trap)
  __builtin_trap();
#else
  __asm__ volatile("movl $0x0, %%eax\n"
                   "movl $0x0, %%ebx\n"
                   "int     $0x80" ::
                       : "eax", "ebx");
  _exit(127 + 6);

#endif
#elif defined(__i386) || defined(__x86) || defined(__x86_64__)
  __asm__ volatile("movl $0x0, %%eax\n"
                   "movl $0x0, %%ebx\n"
                   "int     $0x80" ::
                       : "eax", "ebx");
  _exit(127 + 6);
#else
#error "unsupported platform"
#endif
}

struct free_block* ptr_to_block(void* pointer)
{
  struct free_block* block = pointer - FREE_OFF_BLOCK;
  assert(ISALLOC(block));
  UNSETALLOC(block);
  return block;
}

void* req_new_block(size_t order)
{
  void*              candidate = sbrk(1 << order);
  struct free_block* block     = candidate;

  if (block == (void*) -1) {
    return NULL;
  }

  SETORDER(block, order);

  SETALLOC(block);
  return candidate;
}

void* unlink_free_block(size_t order)
{
  void* candidate        = free_lists[order].next;
  free_lists[order].next = free_lists[order].next->next;
  return candidate;
}

void relink_free_block(struct free_block* block)
{
  int order              = GETORDER(block);
  block->next            = free_lists[order].next;
  free_lists[order].next = block;
}
