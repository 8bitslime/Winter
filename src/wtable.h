/*
** FILE: wtable.h
** 
** PURPOSE: Defines tables and functions used to manipulate them
**
** Copyright (c) 2019 Zachary Wells, see LICENSE for more details
*/

#ifndef WTABLE_H
#define WTABLE_H

#include "winter.h"
#include "wobject.h"
#include "wstring.h"

typedef struct bucket_t {
	char *name; //TODO: use winter string
	unsigned long hash;
	winterObject_t object;
	struct bucket_t *next;
} bucket_t;

typedef struct winterTable_t {
	REFCOUNTED;
	bucket_t **buckets;
	bucket_t  *head;
	size_t numBuckets;
	size_t size;
} winterTable_t;

void _winter_tableAlloc(winterState_t *state, winterTable_t *table, size_t initial);
void _winter_tableFree(winterState_t *state, winterTable_t *table);

void _winter_tableInsert(winterState_t *state, winterTable_t *table, winterString_t *string, const winterObject_t *object);
winterObject_t *_winter_tableGetObject(winterTable_t *table, winterString_t *string);

#endif
