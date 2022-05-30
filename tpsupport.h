//
// Created by Thomas Povinelli on 5/18/22.
//

#ifndef TPMALLOC_TPSUPPORT_H
#define TPMALLOC_TPSUPPORT_H

#include <stdint.h>
#include <unistd.h>

#define FREE_OFF_BLOCK 32  // alignment; must be >sizeof(struct free_block)

#define ISALLOC(block) ((((struct free_block*) (block))->order) & (1 << 7)) != 0
#define SETALLOC(block) ((((struct free_block*) (block))->order) |= (1 << 7))
#define UNSETALLOC(block) ((((struct free_block*) (block))->order) &= ~(1 << 7))
#define SETORDER(block, ord) \
  ASSERT((ord) <= 64);       \
  (((struct free_block*) (block))->order) |= (ord)
#define GETORDER(block) (((struct free_block*) (block))->order) & (~(0xc0))
#define REAL_SIZE(size) ((size) + FREE_OFF_BLOCK)
#define ORDER_FROM_RLSIZE(rlsize) ((sizeof(long) * 8) - LCLZ(((long) (rlsize))))

#define TOSTRING(x) #x
#define STRINGIFY(x) TOSTRING(x)

#define ASSERT(expression)                                                  \
  do {                                                                      \
    if (!(expression)) {                                                    \
      fnputsd(STDERR_FILENO, "Assertion failed: " __FILE__                  \
                             ":" STRINGIFY(__LINE__) " " #expression "\n"); \
      abort();                                                              \
    }                                                                       \
  } while (0)

#define TODO(message)                                                  \
  do {                                                                 \
    fnputsd(STDERR_FILENO, "Not implemented: " __FILE__                \
                           ":" STRINGIFY(__LINE__) " " #message "\n"); \
    abort();                                                           \
  } while (0)

#define UNREACHABLE(...)                                               \
  do {                                                                 \
    fnputsd(STDERR_FILENO, "Expected " __FILE_NAME__ " at " STRINGIFY( \
                               __LINE__) " to be unreachable!\n");     \
    abort();                                                           \
  } while (0)

#define LISTCOUNT 64  // 1 << 1 to 1 << 64

#ifdef __has_builtin
#if __has_builtin(__builtin_lclz)
#define LCLZ __builtin_lclz
#else
#define LCLZ tp_lclz
#endif
#else
#define LCLZ tp_lclz
#endif

#if defined(__cold) && defined(__dead2)
#define ABORT_SUFFIX __cold __dead2
#elif defined(__no_return__)
#define ABORT_SUFFIX __no_return__
#else
#define ABORT_SUFFIX
#endif

struct free_block {
  struct free_block* next;
  struct free_block* prev;
  uint8_t            order;
  // order is log2(size)
  // top two bits are isalloc'd and 0
  // max order is 64 -> max size of alloc block is 1 << 64
} __attribute__((packed));

extern struct free_block free_lists[LISTCOUNT];

void               abort() ABORT_SUFFIX;
void               fnputsd(int fileno, char const* str);
void*              memset(void* pointer, int value, size_t size);
void*              memcpy(void* destination, void const* src, size_t size);
struct free_block* ptr_to_block(void* pointer);
void               puti(int i);
void               putsd(char const* s);
void               relink_free_block(struct free_block* block, unsigned int o);
void*              req_new_block(size_t order);
struct free_block* split_block(struct free_block* block,
                               unsigned int       cur_order,
                               unsigned int       target_order);
long               tp_lclz(long v);
void*              tp_sbrk(size_t increase);
void*              unlink_free_block(size_t order);
struct free_block* pluck_block(struct free_block* block);

#endif  // TPMALLOC_TPSUPPORT_H
