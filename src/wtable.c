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
	table->buckets = MALLOC(sizeof(bucket_t*) * initial);
	table->head = NULL;
	assert(table->buckets != NULL);
	memset(table->buckets, 0, sizeof(bucket_t*) * initial);
	
	table->numBuckets = initial;
	table->size = 0;
}

void _winter_tableFree(winterState_t *state, winterTable_t *table) {
	//TODO: free all objects stored inside table
	while (table->head) {
		bucket_t *temp = table->head->next;
		FREE(table->head);
		table->head = temp;
	}
	printf("\n");
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
	bucket_t *slot = table->buckets[hash];
	
	while (slot && strcmp(name, slot->name)) {
		if (slot->hash % table->numBuckets != hash) {
			return NULL;
		}
		slot = slot->next;
	}
	return slot;
}

void _winter_tableInsert(winterState_t *state, winterTable_t *table, const char *name, const winterObject_t *value) {
	bucket_t *bucket = getBucket(table, name);
	if (bucket == NULL) {
		//Allocate bucket and string in same chunk
		bucket = MALLOC(sizeof(bucket_t) + strlen(name) + 1);
		
		//Set name string to memory after bucket
		bucket->name = (char*)(bucket + 1);
		strcpy(bucket->name, name);
		
		unsigned long hash = hashString(name);
		bucket->hash = hash;
		
		bucket_t **slot = &table->buckets[hash % table->numBuckets];
		
		if (*slot == NULL) {
			//empty slot
			bucket->next = table->head;
			table->head = bucket;
			*slot = bucket;
		} else {
			//occupied slot, insert after first element
			bucket->next = (*slot)->next;
			(*slot)->next = bucket;
		}
	}
	
	if (value == NULL) {
		bucket->object = nullObject;
	} else {
		bucket->object = *value;
	}
}

winterObject_t *_winter_tableGetObject(winterTable_t *table, const char *name) {
	bucket_t *bucket = getBucket(table, name);
	if (bucket) {
		return &bucket->object;
	} else {
		return NULL;
	}
}
