#ifndef OBJECT_H
#define OBJECT_H

#include "winter.h"
#include "lexer.h"

typedef enum object_type_t {
	TYPE_UNKNOWN = 0,
	TYPE_INT,
	TYPE_FLOAT,
	TYPE_STRING,
	TYPE_TABLE,
	TYPE_FUNCTION
} object_type_t;

typedef struct object_t {
	object_type_t type;
	union {
		winterInt_t integer;
		winterFloat_t floating;
		void *pointer;
	};
} object_t;

void _winter_tokenToObject(winterState_t *state, const token_t *token, object_t *dest);

winterInt_t   _winter_castInt(const object_t *object);
winterFloat_t _winter_castFloat(const object_t *object);

#endif
