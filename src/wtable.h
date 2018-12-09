/*
** FILE: wtable.h
** 
** PURPOSE: Defines tables and functions used to manipulate them
**
** Copyright (c) 2018 Zachary Wells, see LICENSE for more details
*/

#ifndef WTABLE_H
#define WTABLE_H

#include "winter.h"

//Possible types of an object
typedef enum object_type_t {
	TYPE_UNKNOWN = 0,
	TYPE_INT, TYPE_FLOAT,
	TYPE_STRING, TYPE_TABLE,
	TYPE_FUNCTION
} object_type_t;

typedef union object_t {
	winterInt_t integer;
	winterFloat_t floating;
	char *string;
	void *pointer;
} object_t;

typedef struct bucket_t {
	char *name;
	object_type_t type;
	object_t value;
} bucket_t;

typedef struct winterTable_t {
	bucket_t *buckets;
	size_t numBuckets;
	size_t size;
} winterTable_t;

void _winter_tableAlloc(winterTable_t *table, size_t initial);

void _winter_tableInsertInt(winterTable_t *table, const char *name, winterInt_t value);
void _winter_tableInsertFloat(winterTable_t *table, const char *name, winterFloat_t value);

winterInt_t _winter_tableToInt(winterTable_t *table, const char *name);
winterFloat_t _winter_tableToFloat(winterTable_t *table, const char *name);

#endif
