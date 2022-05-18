//
// Created by Thomas Povinelli on 5/18/22.
//

#ifndef TPMALLOC_MALLOC_H
#define TPMALLOC_MALLOC_H

#include "tpsupport.h"

void* malloc(size_t size);

void free(void* pointer);

#endif // TPMALLOC_MALLOC_H
