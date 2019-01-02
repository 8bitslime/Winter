/*
** The Winter programming language
** See README.md for more information 
**
** Copyright (c) 2019 Zachary Wells, see LICENSE for more details
*/

#ifndef WINTER_H
#define WINTER_H

#include <stdint.h>

//Primitive types
typedef int64_t winterInt_t;
typedef double  winterFloat_t;

//Allocator function (behaves like realloc)
typedef void * (*winterAllocator_t)(void *pointer, size_t newSize);

//Winter State
typedef struct winterState_t winterState_t;
winterState_t *winterCreateState(winterAllocator_t allocator);
void           winterFreeState(winterState_t *state);

#endif
