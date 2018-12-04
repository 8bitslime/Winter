/*
** FILE: wtable.c
** 
** PURPOSE: Defines tables and functions used to manipulate them
**
** Copyright (c) 2018 Zachary Wells, see LICENSE for more details
*/

#include "wtable.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static unsigned long hashString(const char *string) {
	unsigned long out = 5381, c;
	
	while ((c = *string++)) {
		out = ((out << 5) + out) + c;
	}
	
	return out;
}

void _winter_tableAlloc(winterTable_t *table, size_t initial) {
	table->buckets = calloc(sizeof(bucket_t), initial);
	assert(table->buckets != NULL);
	
	table->numBuckets = initial;
	table->size = 0;
}

static bucket_t *getBucket(winterTable_t *table, const char *name) {
	unsigned long hash = hashString(name) % table->numBuckets;
	bucket_t *slot = &table->buckets[hash];
	
	//TODO: check name
	while (slot->name && strcmp(name, slot->name)) slot++;
	return slot;
}

void _winter_tableInsertInt(winterTable_t *table, const char *name, winterInt_t value) {
	bucket_t *bucket = getBucket(table, name);
	if (bucket != NULL) {
		//TODO: better allocation
		bucket->name = malloc(strlen(name) + 1);
		strcpy(bucket->name, name);
		bucket->type = TYPE_INT;
		bucket->value.integer = value;
	}
}
void _winter_tableInsertFloat(winterTable_t *table, const char *name, winterFloat_t value) {
	bucket_t *bucket = getBucket(table, name);
	if (bucket != NULL) {
		bucket->name = malloc(strlen(name) + 1);
		strcpy(bucket->name, name);
		bucket->type = TYPE_FLOAT;
		bucket->value.floating = value;
	}
	//else realloc
}

winterInt_t _winter_tableToInt(winterTable_t *table, const char *name) {
	bucket_t *bucket = getBucket(table, name);
	if (bucket != NULL && bucket->name != NULL) {
		switch (bucket->type) {
			case TYPE_INT:
				return bucket->value.integer;
			
			case TYPE_FLOAT:
				return (winterInt_t)bucket->value.floating;
				
			default:
				assert(0);
				return 0;
		}
	} else {
		return 0;
	}
}
winterFloat_t _winter_tableToFloat(winterTable_t *table, const char *name) {
	bucket_t *bucket = getBucket(table, name);
	if (bucket != NULL && bucket->name != NULL) {
		switch (bucket->type) {
			case TYPE_INT:
				return (winterFloat_t)bucket->value.integer;
			
			case TYPE_FLOAT:
				return bucket->value.floating;
				
			default:
				assert(0);
				return 0;
		}
	} else {
		return 0;
	}
}
