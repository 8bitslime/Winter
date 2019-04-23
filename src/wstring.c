#include <string.h>
#include "wstring.h"

hash_t _winter_stringHash(wstring_t *string) {
	if (!string->hash) {
		string->hash = _winter_hashCStr(string->data);
	}
	return string->hash;
}

wstring_t *_winter_stringAlloc(winterState_t *state, size_t size) {
	wstring_t *out = state->allocator(NULL, sizeof(wstring_t) + size);
	out->capacity = size;
	out->length   = 0;
	out->data = (char*)(out + 1);
}
wstring_t *_winter_stringCreateSize(winterState_t *state, const char *string, size_t size) {
	wstring_t *out = _winter_stringAlloc(state, size + 1);
	memcpy(out->data, string, size);
	out->data[size] = '\0';
	out->length = size;
	return out;
}
wstring_t *_winter_stringCreate(winterState_t *state, const char *string) {
	return _winter_stringCreateSize(state, string, strlen(string));
}

bool_t _winter_stringCompare(wstring_t *a, wstring_t *b) {
	if (a == b) {
		return true;
	} else if ((_winter_stringHash(a) == _winter_stringHash(b)) && (a->length == b->length)) {
		for (size_t i = 0; a->data[i] == b->data[i]; i++) {
			if (i == a->length - 1) return true;
		}
	}
	return false;
}

void _winter_stringFree(winterState_t *state, wstring_t *string) {
	state->allocator(string, 0);
}
