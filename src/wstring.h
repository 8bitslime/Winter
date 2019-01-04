/*
** FILE: wstring.h
** 
** PURPOSE: Strings management functions
**
** Copyright (c) 2019 Zachary Wells, see LICENSE for more details
*/

#ifndef WSTRING_H
#define WSTRING_H

#include "winter.h"
#include "wtype.h"

unsigned long _winter_stringHash(const char *string);

//TYPE_STRING
typedef struct winterString_t {
	REFCOUNTED; //reference counted object
	unsigned long hash;     //hash of the string
	bool_t        rehash;   //set flag if string has been modified since its hash
	size_t        length;   //length of string, of course
	size_t        capacity; //allocated size
	char         *string;   //null terminated string
} winterString_t;

winterString_t *_winter_stringAlloc(winterState_t *state, size_t length);
winterString_t *_winter_stringCreateLength(winterState_t *state, const char *string, size_t length);
winterString_t *_winter_stringCreate(winterState_t *state, const char *string);

int _winter_stringCmp(winterString_t *s1, winterString_t *s2);

void _winter_stringFree(winterState_t *state, winterString_t *string);

#endif
