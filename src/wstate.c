/*
** FILE: wstate.c
**
** PURPOSE: Winter state management
**
** Copyright (c) 2018 Zachary Wells, see LICENSE for more details
*/

#include "winter.h"
#include "wstate.h"

winterState_t *winterCreateState(winterAllocator_t allocator) {
	winterState_t *ret = allocator(NULL, sizeof(winterState_t));
	if (ret == NULL) {
		//Memory allocation failure
		return NULL;
	}
	ret->allocator = allocator;
	_winter_tableAlloc(allocator, &ret->globalState, 25);
	return ret;
}

void winterFreeState(winterState_t *state) {
	state->allocator(state, 0);
}
