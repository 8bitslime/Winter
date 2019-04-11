#ifndef OBJECT_H
#define OBJECT_H

#include "winter.h"
#include "lexer.h"

//Object related error:
#define OBJECT_OK          0
#define OBJECT_ERROR_TYPE -1

typedef enum object_type_t {
	TYPE_UNKNOWN = 0,
	TYPE_NULL,
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

//Takes the result of a + b and stores it in a
//Also does string concatenation
int _winter_objectAdd(object_t *a, object_t *b);
int _winter_objectSub(object_t *a, object_t *b);
int _winter_objectMul(object_t *a, object_t *b);
int _winter_objectDiv(object_t *a, object_t *b);
int _winter_objectMod(object_t *a, object_t *b);

int _winter_objectNegate(object_t *a);

#endif
