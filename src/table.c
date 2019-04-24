#include "table.h"
#include <string.h>

table_t *_winter_tableAlloc(winterState_t *state, size_t capacity) {
	table_t *out = MALLOC(sizeof(table_t));
	out->_refcount  = 0;
	out->buckets    = MALLOC(sizeof(bucket_t*) * capacity);
	memset(out->buckets, 0, sizeof(bucket_t*) * capacity);
	out->head       = NULL;
	out->numBuckets = capacity;
	out->size       = 0;
	return out;
}
void _winter_tableFree(winterState_t *state, table_t *table) {
	while (table->head != NULL) {
		bucket_t *temp = table->head;
		table->head = table->head->next;
		FREE(temp);
	}
	if (table->buckets != NULL) {
		FREE(table->buckets);
	}
	FREE(table);
}

static inline bucket_t *getBucket(table_t *table, wstring_t *name) {
	hash_t hash = _winter_stringHash(name) % table->numBuckets;
	bucket_t *bucket = table->buckets[hash];
	while (bucket && !_winter_stringCompare(name, bucket->name)) {
		bucket = bucket->next;
		if (bucket && _winter_stringHash(bucket->name) % table->numBuckets != hash) {
			bucket = NULL;
			break;
		}
	}
	return bucket;
}

void _winter_tableInsert(winterState_t *state, table_t *table, wstring_t *name, object_t *value) {
	bucket_t *bucket = getBucket(table, name);
	if (bucket == NULL) {
		//allocate new bucket for value
		bucket = MALLOC(sizeof(bucket_t));
		bucket->name = name;
		hash_t hash = _winter_stringHash(name) % table->numBuckets;
		bucket_t **slot = &table->buckets[hash];
		
		if (*slot == NULL) {
			*slot = bucket;
			bucket->next = table->head;
			table->head = bucket;
		} else {
			bucket->next = (*slot)->next;
			(*slot)->next = bucket;
		}
	}
	bucket->value = *value;
	table->size++;
}

object_t *_winter_tableGetObject(table_t *table, wstring_t *name) {
	bucket_t *bucket = getBucket(table, name);
	if (bucket) {
		return &bucket->value;
	} else {
		return NULL;
	}
}
