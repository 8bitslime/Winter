/*
** FILE: wstate.c
**
** PURPOSE: Winter state management
**
** Copyright (c) 2018 Zachary Wells, see LICENSE for more details
*/

#include "winter.h"
#include "wstate.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

winterState_t *winterCreateState(winterAllocator_t allocator) {
	winterState_t *ret = allocator(NULL, sizeof(winterState_t));
	if (ret == NULL) {
		//Memory allocation failure
		return NULL;
	}
	ret->allocator = allocator;
	_winter_tableAlloc(ret, &ret->globalState, 3);
	ret->errorString = NULL;
	return ret;
}

void winterFreeState(winterState_t *state) {
	_winter_tableFree(state, &state->globalState);
	FREE(state);
}

void _winter_stateError(winterState_t *state, const char *format, ...) {
	//Might leads to stack size problems on embedded systems, we'll see when we get there
	char buffer[1024];
	
	va_list args;
	va_start(args, format);
	int length = vsnprintf(buffer, 1024, format, args);
	va_end(args);
	
	if (state->errorString) {
		FREE(state->errorString);
	}
	
	state->errorString = MALLOC(length + 1);
	strcpy(state->errorString, buffer);
}
