/*
** FILE: wstate.c
**
** PURPOSE: Winter state management
**
** Copyright (c) 2018 Zachary Wells, see LICENSE for more details
*/

#ifndef WSTATE_H
#define WSTATE_H

#include "winter.h"
#include "wtable.h"

typedef struct winterState_t {
	winterAllocator_t allocator;
	winterTable_t globalState;
} winterState_t;

//Ease of use macros
#define MALLOC(size)       (state->allocator(NULL, size))
#define REALLOC(ptr, size) (state->allocator(ptr, size))
#define FREE(ptr)          (state->allocator(ptr, 0))

#endif
