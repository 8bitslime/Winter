#ifndef WSTRING_H
#define WSTRING_H

//TODO unicode
#include "wtype.h"
#include "object.h"

typedef struct wstring_t {
	REFCOUNT;
	hash_t hash;
	size_t length;
	size_t capacity;
	char  *data;
} wstring_t;

//Creates an automatic string with a literal value (no allocations)
#define AUTO_STRING(str) (wstring_t){REF_PERSISTENT, 0, (sizeof(str)/sizeof(*str)), 0, str}

wstring_t *_winter_stringAlloc(winterState_t *state, size_t size);
wstring_t *_winter_stringCreateSize(winterState_t *state, const char *string, size_t size);
wstring_t *_winter_stringCreate(winterState_t *state, const char *string);
void _winter_stringFree(winterState_t *state, wstring_t *string);

hash_t _winter_stringHash(wstring_t *string);
bool_t _winter_stringCompare(wstring_t *a, wstring_t *b);
wstring_t *_winter_stringDup(winterState_t *state, const wstring_t *string);
wstring_t *_winter_stringCat(winterState_t *state, wstring_t *a, wstring_t *b);



#endif
