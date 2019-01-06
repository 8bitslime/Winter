/*
** FILE: wstring.c
** 
** PURPOSE: Strings management functions
**
** Copyright (c) 2019 Zachary Wells, see LICENSE for more details
*/

#include "wstring.h"
#include "wstate.h"

#include <string.h>

unsigned long _winter_stringHash(const char *string) {
	unsigned long out = 5381, c;
	while ((c = *string++)) {
		out = ((out << 5) + out) + c;
	}
	return out;
}

winterString_t *_winter_stringAlloc(winterState_t *state, size_t length) {
	winterString_t *ret = MALLOC(sizeof(winterString_t) + length);
	
	if (unlikely(ret == NULL)) {
		_winter_stateError(state, "memory allocation failed");
		return NULL;
	}
	
	ret->refcount = 0;
	ret->rehash = true;
	ret->string = (char*)(ret + 1);
	ret->length = 0;
	ret->capacity = length;
	
	return ret;
}
winterString_t *_winter_stringCreateLength(winterState_t *state, const char *string, size_t length) {
	winterString_t *ret = _winter_stringAlloc(state, length + 1);
	
	if (unlikely(ret == NULL)) {
		return NULL;
	}
	
	memcpy(ret->string, string, length);
	ret->string[length] = '\0';
	ret->hash = _winter_stringHash(ret->string);
	ret->rehash = false;
	ret->length = length;
	
	return ret;
}
winterString_t *_winter_stringCreate(winterState_t *state, const char *string) {
	size_t length = strlen(string);
	winterString_t *ret = _winter_stringCreateLength(state, string, length);
	return ret;
}

bool_t _winter_stringCmp(winterString_t *s1, winterString_t *s2) {
	if (s1 == s2) {
		return true;
	} else if (s1->length == s2->length) {
		return !strncmp(s1->string, s2->string, s1->length);
	}
	return false;
}

void _winter_stringFree(winterState_t *state, winterString_t *string) {
	FREE(string);
}
