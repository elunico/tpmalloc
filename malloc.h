//
// Created by Thomas Povinelli on 5/18/22.
//

#ifndef TPMALLOC_MALLOC_H
#define TPMALLOC_MALLOC_H

typedef unsigned long size_t;

#define NULL ((void*) 0)

void* memset(void* dest, int c, size_t len);

void* malloc(size_t size);
void* calloc(size_t size, size_t count);
void* realloc(void* pointer, size_t new_size);

void free(void* pointer);

#endif // TPMALLOC_MALLOC_H
