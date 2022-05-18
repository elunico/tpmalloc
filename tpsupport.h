//
// Created by Thomas Povinelli on 5/18/22.
//

#ifndef TPMALLOC_TPSUPPORT_H
#define TPMALLOC_TPSUPPORT_H

#include <assert.h>
#include <stdint.h>
#include <unistd.h>

#define LISTCOUNT 12

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
#define ABORT_SUFFIX __code __dead2
#elif defined(__no_return__)
#define ABORT_SUFFIX __no_return__
#else
#define ABORT_SUFFIX
#endif

#define ISALLOC(block) (((struct free_block*) (block))->flags & (1 << 7)) != 0
#define ISSENT(block) (((struct free_block*) (block))->flags & (1 << 3)) == 0
#define SETNOTSENT(block) (((struct free_block*) (block))->flags | (1 << 3))
#define SETALLOC(block) (((struct free_block*) (block))->flags |= (1 << 7))
#define UNSETALLOC(block) (((struct free_block*) (block))->flags &= ~(1 << 7))
#define SETMAGIC(block) (((struct free_block*) (block))->flags |= 96)
#define ISMAGIC(block) (((struct free_block*) (block))->flags & 96) != 0
#define SETORDER(block, order) (((struct free_block*) (block))->order) = (order)

#define REAL_SIZE(size) ((size) + sizeof(struct free_block))

#define ORDER_FROM_RLSIZE(rlsize) ((sizeof(rlsize) * 8) - LCLZ(rlsize))

void               abort() ABORT_SUFFIX;
void*              unlink_free_block(size_t order);
void*              req_new_block(size_t order);
struct free_block* ptr_to_block(void* pointer);
void               relink_free_block(struct free_block* block);
long               tp_lclz(long v);


struct free_block {
  uint8_t order;
  // size of block = 1 << order
  uint8_t flags;
  // 0       0 0 0 0        0 0 0
  // alloc'd 1 1 0 !sentinel X X X
  struct free_block* next;
  struct free_block* prev;
};

extern struct free_block free_lists[LISTCOUNT];


#endif // TPMALLOC_TPSUPPORT_H
