#include <string.h>
#include "wstring.h"

hash_t _winter_stringHash(wstring_t *string) {
	if (!string->hash) {
		string->hash = _winter_hashCStr(string->data);
	}
	return string->hash;
}

wstring_t *_winter_stringAlloc(winterState_t *state, size_t size) {
	wstring_t *out = MALLOC(sizeof(wstring_t) + size);
	out->_refcount = 0;
	out->hash      = 0;
	out->length    = 0;
	out->capacity  = size;
	out->data = (char*)(out + 1);
	return out;
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
void _winter_stringFree(winterState_t *state, wstring_t *string) {
	FREE(string);
}

wstring_t *_winter_stringDup(winterState_t *state, const wstring_t *string) {
	wstring_t *out = _winter_stringCreateSize(state, string->data, string->length);
	return out;
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
