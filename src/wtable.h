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
#include "wobject.h"

typedef struct bucket_t {
	char *name;
	winterObject_t object;
} bucket_t;

typedef struct winterTable_t {
	bucket_t *buckets;
	size_t numBuckets;
	size_t size;
} winterTable_t;

void _winter_tableAlloc(winterAllocator_t allocator, winterTable_t *table, size_t initial);

void _winter_tableInsert(winterAllocator_t allocator, winterTable_t *table, const char *name, const winterObject_t *object);
void _winter_tableInsertInt(winterAllocator_t allocator, winterTable_t *table, const char *name, winterInt_t value);
void _winter_tableInsertFloat(winterAllocator_t allocator, winterTable_t *table, const char *name, winterFloat_t value);

winterObject_t *_winter_tableGetObject(winterTable_t *table, const char *name);
winterInt_t _winter_tableToInt(winterTable_t *table, const char *name);
winterFloat_t _winter_tableToFloat(winterTable_t *table, const char *name);

#endif
