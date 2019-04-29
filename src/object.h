#ifndef OBJECT_H
#define OBJECT_H

#include "wtype.h"
#include "lexer.h"

//Object related error:
#define OBJECT_OK          0
#define OBJECT_ERROR_TYPE -1

#define isRefCounted(t) ((t) >= TYPE_STRING)

//Add to beginning of all ref counted types
#define REFCOUNT uint32_t _refcount
#define REF_PERSISTENT 0xffffffff

typedef struct refcount_t {
	REFCOUNT;
} refcount_t;

typedef enum object_type_t {
	TYPE_UNKNOWN = 0,
	TYPE_NULL,
	TYPE_REFERENCE,
	TYPE_INT,
	TYPE_FLOAT,
	TYPE_STRING,
	TYPE_TABLE,
	TYPE_FUNCTION
} object_type_t;

typedef struct object_t {
	object_type_t type;
	union {
		winterInt_t integer;
		winterFloat_t floating;
		void *pointer;
	};
} object_t;

typedef unsigned long hash_t;
hash_t _winter_hashCStr(const char *string);
hash_t _winter_hashObjet(object_t *obj);

void _winter_tokenToObject(winterState_t *state, const token_t *token, object_t *dest);

winterInt_t   _winter_castInt(const object_t *object);
winterFloat_t _winter_castFloat(const object_t *object);

object_t *_winter_objectAddRef(winterState_t *state, object_t *obj);
object_t *_winter_objectDelRef(winterState_t *state, object_t *obj);

bool_t _winter_objectComp(object_t *a, object_t *b);

//Takes the result of a + b and stores it in a
//Also does string concatenation
int _winter_objectAdd(winterState_t *state, object_t *a, object_t *b);
int _winter_objectSub(winterState_t *state, object_t *a, object_t *b);
int _winter_objectMul(winterState_t *state, object_t *a, object_t *b);
int _winter_objectDiv(winterState_t *state, object_t *a, object_t *b);
int _winter_objectMod(winterState_t *state, object_t *a, object_t *b);
int _winter_objectPow(winterState_t *state, object_t *a, object_t *b);

int _winter_objectAssign(winterState_t *state, object_t *a, object_t *b);

int _winter_objectNegate(winterState_t *state, object_t *a);

#endif
