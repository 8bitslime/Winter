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

#endif
