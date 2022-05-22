//
// Created by Thomas Povinelli on 5/18/22.
//
#include "tpsupport.h"
#include <sys/mman.h>

struct free_block free_lists[LISTCOUNT];

void abort()
{
#if defined(__has_builtin)
#if __has_builtin(__builtin_trap)
  __builtin_trap();
#elif defined(__i386) || defined(__x86) || defined(__x86_64__)
  __asm__ volatile(
      "movl $0x0, %%eax\n"
      "movl $0x0, %%ebx\n"
      "int     $0x80" ::
          : "eax", "ebx");
  _exit(127 + 6);
#else
#error "unsupported platform"
#endif
#elif defined(__i386) || defined(__x86) || defined(__x86_64__)
  __asm__ volatile(
      "movl $0x0, %%eax\n"
      "movl $0x0, %%ebx\n"
      "int     $0x80" ::
          : "eax", "ebx");
  _exit(127 + 6);
#else
#error "unsupported platform"
#endif
}

ssize_t fnputch(int fileno, char c)
{
  return write(fileno, &c, 1);
}

void fnputsd(int fno, char const* str)
{
  while (*str)
    fnputch(fno, *str++);
}

void* memcpy(void* destination, void const* src, size_t size)
{
  for (size_t i = 0; i < size; i++)
    ((char*) destination)[i] = ((char*) src)[i];
  return destination;
}

void* memset(void* pointer, int value, size_t size)
{
  char* p = pointer;
  for (size_t i = 0; i < size; i++)
    p[i] = value;
  return pointer;
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

void putsd(char const* s)
{
  fnputsd(STDOUT_FILENO, s);
}

struct free_block* ptr_to_block(void* pointer)
{
  struct free_block* block = pointer - FREE_OFF_BLOCK;
  ASSERT(ISALLOC(block));
  UNSETALLOC(block);
  return block;
}

void relink_free_block(struct free_block* block)
{
  int order              = GETORDER(block);
  block->next            = free_lists[order].next;
  free_lists[order].next = block;
}

void* req_new_block(size_t order)
{
  void*              candidate = tp_sbrk(1 << order);
  struct free_block* block     = candidate;

  if (block == (void*) -1) {
    return NULL;
  }

  SETORDER(block, order);

  SETALLOC(block);
  return candidate;
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

size_t strlen(char const* s)
{
  size_t c = 0;
  while (*s++)
    c++;
  return c;
}

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

void* tp_sbrk(size_t increase)
{
  return mmap(NULL, increase, PROT_READ | PROT_WRITE,
              MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

void* unlink_free_block(size_t order)
{
  void* candidate        = free_lists[order].next;
  free_lists[order].next = free_lists[order].next->next;
  return candidate;
}
