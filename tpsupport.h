//
// Created by Thomas Povinelli on 5/18/22.
//

#ifndef TPMALLOC_TPSUPPORT_H
#define TPMALLOC_TPSUPPORT_H

#include <stdint.h>
#include <unistd.h>

void fnputsd(int fileno, char const* str);

void putsd(char const* s);
void puti(int i);

#define assert(expression)                                                                                             \
  do {                                                                                                                 \
    if (!(expression)) {                                                                                               \
      fnputsd(STDERR_FILENO, "Assertion failed: " #expression "\n");                                                   \
      abort();                                                                                                         \
    }                                                                                                                  \
  } while (0)


#define LISTCOUNT 64 // 1 << 1 to 1 << 64

#ifdef __has_builtin
#if __has_builtin(__builtin_lclz)
#define LCLZ __builtin_lclz
#else
#define LCLZ tp_lclz
#endif
#else
#define LCLZ tp_lclz
#endif

#define FREE_OFF_BLOCK 16 // alignment purposes must be >sizeof(struct free_block)

#if defined(__cold) && defined(__dead2)
#define ABORT_SUFFIX __cold __dead2
#elif defined(__no_return__)
#define ABORT_SUFFIX __no_return__
// #elif defined(__attribute__((noreturn)))
// #define ABORT_SUFFIX __attribute__((noreturn))
#else
#define ABORT_SUFFIX
#endif

#define ISALLOC(block) ((((struct free_block*) (block))->order) & (1 << 7)) != 0
#define SETALLOC(block) ((((struct free_block*) (block))->order) |= (1 << 7))
#define UNSETALLOC(block) ((((struct free_block*) (block))->order) &= ~(1 << 7))
#define SETORDER(block, ord) assert((ord) <= 64); (((struct free_block*) (block))->order) |= (ord)
#define GETORDER(block) (((struct free_block*) (block))->order) & (~(0xc0))

#define REAL_SIZE(size) ((size) + FREE_OFF_BLOCK)

#define ORDER_FROM_RLSIZE(rlsize) ((sizeof(long) * 8) - LCLZ(((long)(rlsize))))

void               abort() ABORT_SUFFIX;
void*              unlink_free_block(size_t order);
void*              req_new_block(size_t order);
struct free_block* ptr_to_block(void* pointer);
void               relink_free_block(struct free_block* block);
long               tp_lclz(long v);


struct free_block {
  struct free_block* next;
  uint8_t order;
  // order is log2(size)
  // top two bits are isalloc'd and 0
  // max order is 64 -> max size of alloc block is 1 << 64
}__attribute__((packed));

extern struct free_block free_lists[LISTCOUNT];


#endif // TPMALLOC_TPSUPPORT_H
