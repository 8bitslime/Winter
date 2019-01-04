/*
** FILE: wtable.c
** 
** PURPOSE: Defines tables and functions used to manipulate them
**
** Copyright (c) 2019 Zachary Wells, see LICENSE for more details
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
		_winter_objectDelRef(state, &table->head->object);
		FREE(table->head);
		table->head = temp;
	}
	printf("\n");
	FREE(table->buckets);
	memset(table, 0, sizeof(winterTable_t));
}

static bucket_t *getBucket(winterTable_t *table, winterString_t *string) {
	if (string->rehash) {
		printf("rehashing\n");
		string->hash = _winter_stringHash(string->string);
	}
	
	unsigned long hash = string->hash % table->numBuckets;
	bucket_t *slot = table->buckets[hash];
	
	while (slot && strcmp(string->string, slot->name)) {
		if (slot->hash % table->numBuckets != hash) {
			return NULL;
		}
		slot = slot->next;
	}
	return slot;
}

void _winter_tableInsert(winterState_t *state, winterTable_t *table, winterString_t *string, const winterObject_t *value) {
	bucket_t *bucket = getBucket(table, string);
	if (bucket == NULL) {
		//Allocate bucket and string in same chunk
		bucket = MALLOC(sizeof(bucket_t) + string->length + 1);
		
		//Set name string to memory after bucket
		bucket->name = (char*)(bucket + 1);
		strcpy(bucket->name, string->string);
		
		bucket->hash = string->hash;
		
		bucket_t **slot = &table->buckets[string->hash % table->numBuckets];
		
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
		bucket->object = *_winter_objectAddRef(state, value);
	}
}

winterObject_t *_winter_tableGetObject(winterTable_t *table, winterString_t *string) {
	bucket_t *bucket = getBucket(table, string);
	printf("finding thing: %s\n", string->string);
	if (bucket != NULL) {
		printf("found!\n");
		return &bucket->object;
	} else {
		return NULL;
	}
}
