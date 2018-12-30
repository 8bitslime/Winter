/*
** FILE: wobject.h
**
** PURPOSE: Objects in the Winter interpreter
**
** Copyright (c) 2018 Zachary Wells, see LICENSE for more details
*/

#ifndef WOBJECT_H
#define WOBJECT_H

#include "winter.h"
#include "wtype.h"

typedef enum object_type_t {
	TYPE_NULL = 0,
	TYPE_INT, TYPE_FLOAT, TYPE_TABLE, TYPE_STRING, TYPE_POINTER, TYPE_REF
} object_type_t;

typedef struct winterObject_t {
	object_type_t type;
	union {
		winterInt_t integer;
		winterFloat_t floating;
		char *string;
		void *pointer;
	};
} winterObject_t;

static const winterObject_t nullObject = {
	TYPE_NULL,
	{0}
};

//Returns zero if a and b are incompatible objects
//Result stored in dest
//All paramters are allowed to point to the same object
bool_t _winter_objectToInt(winterObject_t *dest, const winterObject_t *a);

bool_t _winter_objectAdd(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b);
bool_t _winter_objectSub(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b);
bool_t _winter_objectMul(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b);
bool_t _winter_objectDiv(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b);
bool_t _winter_objectMod(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b);
bool_t _winter_objectPow(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b);
bool_t _winter_objectEqual(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b);

bool_t _winter_objectAssign(winterObject_t *dest, winterObject_t *a, const winterObject_t *b);
bool_t _winter_objectAddEq(winterObject_t *dest, winterObject_t *a, const winterObject_t *b);
bool_t _winter_objectMinEq(winterObject_t *dest, winterObject_t *a, const winterObject_t *b);
bool_t _winter_objectMulEq(winterObject_t *dest, winterObject_t *a, const winterObject_t *b);
bool_t _winter_objectDivEq(winterObject_t *dest, winterObject_t *a, const winterObject_t *b);

bool_t _winter_objectPreInc(winterObject_t *dest, winterObject_t *a);
bool_t _winter_objectPreDec(winterObject_t *dest, winterObject_t *a);
bool_t _winter_objectNegate(winterObject_t *dest, const winterObject_t *a);
bool_t _winter_objectNot(winterObject_t *dest, const winterObject_t *a);

#endif
