#ifndef WSTRING_H
#define WSTRING_H

//TODO unicode
#include "winter.h"
#include "wtype.h"
#include "object.h"

typedef struct wstring_t {
	REFCOUNT;
	hash_t hash;
	size_t length;
	size_t capacity;
	char  *data;
} wstring_t;

wstring_t *_winter_stringAlloc(winterState_t *state, size_t size);
wstring_t *_winter_stringCreateSize(winterState_t *state, const char *string, size_t size);
wstring_t *_winter_stringCreate(winterState_t *state, const char *string);

hash_t _winter_stringHash(wstring_t *string);
bool_t _winter_stringCompare(wstring_t *a, wstring_t *b);

void _winter_stringFree(winterState_t *state, wstring_t *string);

#endif
