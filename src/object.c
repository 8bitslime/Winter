#include "object.h"

#define typeof(o) (o->type)

//djb2 by Dan Bernstein.
hash_t _winter_hashCStr(const char *string) {
	hash_t out = 5381, c;
	while ((c = *string++)) {
		out = ((out << 5) + out) + c;
	}
	
	if (out == 0) { //reserved hash value
		out = 1;
	}
	
	return out;
}

void _winter_tokenToObject(winterState_t *state, const token_t *token, object_t *dest) {
	dest->type = TYPE_INT;
	switch (typeof(token)) {
		case TK_BINARY:
			dest->integer = winter_strtoi(token->cursor.pointer + 2, 2);
			break;
		case TK_OCTAL:
			dest->integer = winter_strtoi(token->cursor.pointer + 1, 8);
			break;
		case TK_DECIMAL:
			dest->integer = winter_strtoi(token->cursor.pointer, 10);
			break;
		case TK_HEX:
			dest->integer = winter_strtoi(token->cursor.pointer + 2, 16);
			break;
		
		case TK_FLOAT:
			dest->floating = winter_strtof(token->cursor.pointer);
			dest->type = TYPE_FLOAT;
			break;
		
		default:
			*dest = (object_t){0};
			break;
	}
	return;
}

winterInt_t _winter_castInt(const object_t *object) {
	switch (typeof(object)) {
		case TYPE_INT:
			return object->integer;
		
		case TYPE_FLOAT:
			return (winterInt_t)object->floating;
		
		default: return 0;
	}
}

winterFloat_t _winter_castFloat(const object_t *object) {
	switch (typeof(object)) {
		case TYPE_INT:
			return (winterFloat_t)object->integer;
		
		case TYPE_FLOAT:
			return object->floating;
		
		default: return 0;
	}
}

int _winter_objectAdd(object_t *a, object_t *b) {
	if (typeof(a) > TYPE_NULL && typeof(b) > TYPE_NULL) {
		if (typeof(a) > TYPE_STRING || typeof(b) > TYPE_STRING) {
			//error
			return OBJECT_ERROR_TYPE;
		} else if (typeof(a) == TYPE_STRING || typeof(b) == TYPE_STRING) {
			//string concatenation
			return OBJECT_OK;
		} else if (typeof(a) == TYPE_FLOAT || typeof(b) == TYPE_FLOAT) {
			a->floating = _winter_castFloat(a) + _winter_castFloat(b);
			return OBJECT_OK;
		} else if (typeof(a) == TYPE_INT && typeof(b) == TYPE_INT) {
			a->integer += b->integer;
			return OBJECT_OK;
		}
	}
	return OBJECT_ERROR_TYPE;
}
int _winter_objectSub(object_t *a, object_t *b) {
	if (typeof(a) > TYPE_NULL && typeof(b) > TYPE_NULL) {
		if (typeof(a) >= TYPE_STRING || typeof(b) >= TYPE_STRING) {
			//error
			return OBJECT_ERROR_TYPE;
		} else if (typeof(a) == TYPE_FLOAT || typeof(b) == TYPE_FLOAT) {
			a->floating = _winter_castFloat(a) - _winter_castFloat(b);
			return OBJECT_OK;
		} else if (typeof(a) == TYPE_INT && typeof(b) == TYPE_INT) {
			a->integer -= b->integer;
			return OBJECT_OK;
		}
	}
	return OBJECT_ERROR_TYPE;
}
int _winter_objectMul(object_t *a, object_t *b) {
	if (typeof(a) > TYPE_NULL && typeof(b) > TYPE_NULL) {
		if (typeof(a) >= TYPE_STRING || typeof(b) >= TYPE_STRING) {
			//error
			return OBJECT_ERROR_TYPE;
		} else if (typeof(a) == TYPE_FLOAT || typeof(b) == TYPE_FLOAT) {
			a->floating = _winter_castFloat(a) * _winter_castFloat(b);
			return OBJECT_OK;
		} else if (typeof(a) == TYPE_INT && typeof(b) == TYPE_INT) {
			a->integer *= b->integer;
			return OBJECT_OK;
		}
	}
	return OBJECT_ERROR_TYPE;
}
int _winter_objectDiv(object_t *a, object_t *b) {
	if (typeof(a) > TYPE_NULL && typeof(b) > TYPE_NULL) {
		if (typeof(a) >= TYPE_STRING || typeof(b) >= TYPE_STRING) {
			//error
			return OBJECT_ERROR_TYPE;
		} else if (typeof(a) == TYPE_FLOAT || typeof(b) == TYPE_FLOAT) {
			a->floating = _winter_castFloat(a) / _winter_castFloat(b);
			return OBJECT_OK;
		} else if (typeof(a) == TYPE_INT && typeof(b) == TYPE_INT) {
			a->integer /= b->integer;
			return OBJECT_OK;
		}
	}
	return OBJECT_ERROR_TYPE;
}
int _winter_objectMod(object_t *a, object_t *b) {
	if (typeof(a) > TYPE_NULL && typeof(b) > TYPE_NULL) {
		if (typeof(a) >= TYPE_STRING || typeof(b) >= TYPE_STRING) {
			//error
			return OBJECT_ERROR_TYPE;
		} else if (typeof(a) == TYPE_FLOAT || typeof(b) == TYPE_FLOAT) {
			a->floating = 0;
			return OBJECT_OK;
		} else if (typeof(a) == TYPE_INT && typeof(b) == TYPE_INT) {
			a->integer %= b->integer;
			return OBJECT_OK;
		}
	}
	return OBJECT_ERROR_TYPE;
}

int _winter_objectNegate(object_t *a) {
	switch(typeof(a)) {
		case TYPE_FLOAT:
			a->floating *= -1;
			return OBJECT_OK;
		case TYPE_INT:
			a->integer *= -1;
			return OBJECT_OK;
		default: return OBJECT_ERROR_TYPE;
	}
}
