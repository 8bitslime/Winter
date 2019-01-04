/*
** FILE: wobject.h
**
** PURPOSE: Objects in the Winter interpreter
**
** Copyright (c) 2019 Zachary Wells, see LICENSE for more details
*/

#ifndef WOBJECT_H
#define WOBJECT_H

#include "winter.h"
#include "wtype.h"
#include "wstring.h"

typedef enum object_type_t {
	TYPE_NULL = 0,
	TYPE_INT, TYPE_FLOAT, TYPE_REF, TYPE_TABLE, TYPE_STRING, TYPE_POINTER
} object_type_t;


typedef struct refcounted_t {
	REFCOUNTED;
} refcounted_t;

typedef struct winterObject_t {
	object_type_t type;
	union {
		winterInt_t integer;
		winterFloat_t floating;
		winterString_t *string;
		void *pointer;
	};
} winterObject_t;

static const winterObject_t nullObject = {
	TYPE_NULL,
	{0}
};

//Adds reference to object then returns object
winterObject_t *_winter_objectAddRef(winterState_t *state, winterObject_t *object);

//Removes reference from object and returns object
//If the data in object was freed, then object will be set to TYPE_NULL
winterObject_t *_winter_objectDelRef(winterState_t *state, winterObject_t *object);

//Returns false if a and b are incompatible objects
//Result stored in dest
//All paramters are allowed to point to the same object
bool_t _winter_objectToInt(winterState_t *state, winterObject_t *dest, const winterObject_t *a);

bool_t _winter_objectAdd(winterState_t *state, winterObject_t *dest, const winterObject_t *a, const winterObject_t *b);
bool_t _winter_objectSub(winterState_t *state, winterObject_t *dest, const winterObject_t *a, const winterObject_t *b);
bool_t _winter_objectMul(winterState_t *state, winterObject_t *dest, const winterObject_t *a, const winterObject_t *b);
bool_t _winter_objectDiv(winterState_t *state, winterObject_t *dest, const winterObject_t *a, const winterObject_t *b);
bool_t _winter_objectMod(winterState_t *state, winterObject_t *dest, const winterObject_t *a, const winterObject_t *b);
bool_t _winter_objectPow(winterState_t *state, winterObject_t *dest, const winterObject_t *a, const winterObject_t *b);
bool_t _winter_objectEqual(winterState_t *state, winterObject_t *dest, const winterObject_t *a, const winterObject_t *b);

bool_t _winter_objectAssign(winterState_t *state, winterObject_t *dest, winterObject_t *a, const winterObject_t *b);
bool_t _winter_objectAddEq(winterState_t *state, winterObject_t *dest, winterObject_t *a, const winterObject_t *b);
bool_t _winter_objectMinEq(winterState_t *state, winterObject_t *dest, winterObject_t *a, const winterObject_t *b);
bool_t _winter_objectMulEq(winterState_t *state, winterObject_t *dest, winterObject_t *a, const winterObject_t *b);
bool_t _winter_objectDivEq(winterState_t *state, winterObject_t *dest, winterObject_t *a, const winterObject_t *b);

bool_t _winter_objectPreInc(winterState_t *state, winterObject_t *dest, winterObject_t *a);
bool_t _winter_objectPreDec(winterState_t *state, winterObject_t *dest, winterObject_t *a);
bool_t _winter_objectNegate(winterState_t *state, winterObject_t *dest, const winterObject_t *a);
bool_t _winter_objectNot(winterState_t *state, winterObject_t *dest, const winterObject_t *a);

#endif
