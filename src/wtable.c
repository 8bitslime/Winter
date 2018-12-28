/*
** FILE: wtable.c
** 
** PURPOSE: Defines tables and functions used to manipulate them
**
** Copyright (c) 2018 Zachary Wells, see LICENSE for more details
*/

#include "wtable.h"
#include "wstate.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void _winter_tableAlloc(winterState_t *state, winterTable_t *table, size_t initial) {
	table->buckets = MALLOC(sizeof(bucket_t) * initial);
	assert(table->buckets != NULL);
	memset(table->buckets, 0, sizeof(bucket_t) * initial);
	
	table->numBuckets = initial;
	table->size = 0;
}

void _winter_tableFree(winterState_t *state, winterTable_t *table) {
	//TODO: free all objects stored inside table
	FREE(table->buckets);
	memset(table, 0, sizeof(winterTable_t));
}

static unsigned long hashString(const char *string) {
	unsigned long out = 5381, c;
	
	while ((c = *string++)) {
		out = ((out << 5) + out) + c;
	}
	
	return out;
}

static bucket_t *getBucket(winterTable_t *table, const char *name) {
	unsigned long hash = hashString(name) % table->numBuckets;
	unsigned long index = hash;
	bucket_t *slot = &table->buckets[hash];
	
	while (slot->name && strcmp(name, slot->name) != 0) {
		index = (index + 1) % table->numBuckets;
		if (index == hash) {
			return NULL;
		}
		slot = &table->buckets[index];
	}
	// printf("%s hashed in slot: %lu\n", name, index);
	return slot;
}

void _winter_tableInsert(winterState_t *state, winterTable_t *table, const char *name, const winterObject_t *value) {
	bucket_t *bucket = getBucket(table, name);
	if (bucket != NULL) {
		if (bucket->name == NULL) {
			bucket->name = MALLOC(strlen(name) + 1);
			strcpy(bucket->name, name);
			table->size++;
		}
		if (value) {
			bucket->object = *value;
		} else {
			bucket->object = (winterObject_t){0};
		}
	}
}
void _winter_tableInsertInt(winterState_t *state, winterTable_t *table, const char *name, winterInt_t value) {
	bucket_t *bucket = getBucket(table, name);
	if (bucket != NULL) {
		if (bucket->name == NULL) {
			bucket->name = MALLOC(strlen(name) + 1);
			strcpy(bucket->name, name);
			table->size++;
		}
		bucket->object.type = TYPE_INT;
		bucket->object.integer = value;
	}
}
void _winter_tableInsertFloat(winterState_t *state, winterTable_t *table, const char *name, winterFloat_t value) {
	bucket_t *bucket = getBucket(table, name);
	if (bucket != NULL) {
		if (bucket->name == NULL) {
			bucket->name = MALLOC(strlen(name) + 1);
			strcpy(bucket->name, name);
			table->size++;
		}
		bucket->object.type = TYPE_FLOAT;
		bucket->object.floating = value;
	}
}


winterObject_t *_winter_tableGetObject(winterTable_t *table, const char *name) {
	bucket_t *bucket = getBucket(table, name);
	if (bucket && bucket->name != NULL) {
		return &bucket->object;
	} else {
		return NULL;
	}
}
winterInt_t _winter_tableToInt(winterTable_t *table, const char *name) {
	bucket_t *bucket = getBucket(table, name);
	if (bucket && bucket->name != NULL) {
		switch (bucket->object.type) {
			case TYPE_INT:
				return bucket->object.integer;
			
			case TYPE_FLOAT:
				return (winterInt_t)bucket->object.floating;
				
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
		switch (bucket->object.type) {
			case TYPE_INT:
				return (winterFloat_t)bucket->object.integer;
			
			case TYPE_FLOAT:
				return bucket->object.floating;
				
			default:
				assert(0);
				return 0;
		}
	} else {
		return 0;
	}
}
