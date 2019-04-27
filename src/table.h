#ifndef TABLE_H
#define TABLE_H

//prevent circular dependencies
typedef struct winterState_t winterState_t;

#include "object.h"
#include "wstring.h"

typedef struct bucket_t {
	wstring_t *name;
	object_t value;
	struct bucket_t *next;
} bucket_t;

typedef struct table_t {
	REFCOUNT;
	bucket_t **buckets;
	bucket_t  *head;
	size_t numBuckets;
	size_t size;
} table_t;

table_t *_winter_tableAlloc(winterState_t *state, size_t capacity);
void _winter_tableFree(winterState_t *state, table_t *table);

object_t *_winter_tableInsert(winterState_t *state, table_t *table, wstring_t *name, object_t *value);
object_t *_winter_tableGetObject(table_t *table, wstring_t *name);

#endif
