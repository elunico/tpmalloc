//
// Created by Thomas Povinelli on 5/18/22.
//

#ifndef TPMALLOC_MALLOC_H
#define TPMALLOC_MALLOC_H

typedef unsigned long size_t;

#define NULL ((void*) 0)

void* calloc(size_t size, size_t count);
void  free(void* pointer);
void* malloc(size_t size);
void* realloc(void* pointer, size_t new_size);

#endif  // TPMALLOC_MALLOC_H
