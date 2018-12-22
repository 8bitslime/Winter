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

typedef enum object_type_t {
	TYPE_NULL = 0,
	TYPE_INT, TYPE_FLOAT, TYPE_TABLE, TYPE_STRING, TYPE_POINTER
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

//Returns zero if a and b are incompatible objects
//Result stored in dest
//All paramters are allowed to point to the same object
int _winter_objectToInt(winterObject_t *dest, const winterObject_t *a);

int _winter_objectAdd(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b);
int _winter_objectSub(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b);
int _winter_objectMul(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b);
int _winter_objectDiv(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b);
int _winter_objectMod(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b);

#endif
