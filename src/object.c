#include "object.h"
#include "wstring.h"
#include "table.h"

#include <math.h>

#define typeof(o) ((o)->type)
#define isNumeric(o) ((o)->type == TYPE_INT || (o)->type == TYPE_FLOAT)

static inline object_t *deref(object_t *obj) {
	if (obj->type == TYPE_REFERENCE) {
		return obj->pointer;
	}
	return obj;
}

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

hash_t _winter_hashObjet(object_t *obj) {
	hash_t out = 1;
	switch (obj->type) {
		case TYPE_STRING:
			out = _winter_stringHash(obj->pointer);
			break;
		
		default: break;
	}
	return out;
}

object_t *_winter_objectAddRef(winterState_t *state, object_t *obj) {
	if (isRefCounted(obj->type)) {
		refcount_t *ref = obj->pointer;
		if (ref->_refcount != REF_PERSISTENT) {
			ref->_refcount += 1;
		}
	}
	return obj;
}
object_t *_winter_objectDelRef(winterState_t *state, object_t *obj) {
	if (isRefCounted(obj->type)) {
		refcount_t *ref = obj->pointer;
		if (ref->_refcount != REF_PERSISTENT) {
			ref->_refcount -= 1;
			if (ref->_refcount == 0) {
				switch (obj->type) {
					case TYPE_STRING: printf("freeing: '%s'\n", obj->string->data); _winter_stringFree(state, obj->pointer); break;
					case TYPE_TABLE:  _winter_tableFree (state, obj->pointer); break;
					default: break;
				}
				return NULL;
			}
		}
	}
	return obj;
}

bool_t _winter_objectComp(object_t *a, object_t *b) {
	bool_t out = false;
	a = deref(a);
	b = deref(b);
	if (a->type == b->type) {
		switch (a->type) {
			case TYPE_UNKNOWN: out = false; break;
			case TYPE_NULL:    out = true;  break;
			case TYPE_INT:     out = a->integer  == b->integer;  break;
			case TYPE_FLOAT:   out = a->floating == b->floating; break;
			case TYPE_STRING:  out = _winter_stringCompare(a->pointer, b->pointer); break;
			default: out = a->pointer == b->pointer; break;
		}
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
		
		case TK_IDENT:
			dest->pointer = _winter_stringCreateSize(state, token->cursor.pointer, token->size);
			dest->type = TYPE_STRING;
			//Add reference to new string
			_winter_objectAddRef(state, dest);
			break;
		
		case TK_STRING:
			//TODO: run through and convert escape sequences
			dest->pointer = _winter_stringCreateSize(state, token->cursor.pointer + 1, token->size - 2);
			dest->type = TYPE_STRING;
			_winter_objectAddRef(state, dest);
			break;
		
		default:
			*dest = (object_t){0};
			break;
	}
	return;
}

winterInt_t _winter_castInt(const object_t *obj) {
	switch (typeof(obj)) {
		case TYPE_INT:
			return obj->integer;
		
		case TYPE_FLOAT:
			return (winterInt_t)obj->floating;
		
		case TYPE_REFERENCE:
			return _winter_castInt(obj->pointer);
		
		default: return 0;
	}
}

winterFloat_t _winter_castFloat(const object_t *obj) {
	switch (typeof(obj)) {
		case TYPE_INT:
			return (winterFloat_t)obj->integer;
		
		case TYPE_FLOAT:
			return obj->floating;
		
		case TYPE_REFERENCE:
			return _winter_castFloat(obj->pointer);
		
		default: return 0;
	}
}

int _winter_objectAdd(winterState_t *state, object_t *a, object_t *b) {
	object_t *out = a;
	a = deref(a);
	b = deref(b);
	if (typeof(a) > TYPE_NULL && typeof(b) > TYPE_NULL) {
		if (typeof(a) > TYPE_STRING || typeof(b) > TYPE_STRING) {
			//error
			return OBJECT_ERROR_TYPE;
		} else if (typeof(a) == TYPE_STRING || typeof(b) == TYPE_STRING) {
			object_t temp;
			temp.type = TYPE_STRING;
			temp.pointer = _winter_stringCat(state, a->string, b->string);
			if (out == a) {
				_winter_objectDelRef(state, a);
			}
			*out = *_winter_objectAddRef(state, &temp);
			return OBJECT_OK;
		} else if (typeof(a) == TYPE_FLOAT || typeof(b) == TYPE_FLOAT) {
			out->type = TYPE_FLOAT;
			out->floating = _winter_castFloat(a) + _winter_castFloat(b);
			return OBJECT_OK;
		} else if (typeof(a) == TYPE_INT && typeof(b) == TYPE_INT) {
			out->type = TYPE_INT;
			out->integer = a->integer + b->integer;
			return OBJECT_OK;
		}
	}
	return OBJECT_ERROR_TYPE;
}
int _winter_objectSub(winterState_t *state, object_t *a, object_t *b) {
	object_t *out = a;
	a = deref(a);
	b = deref(b);
	if (typeof(a) > TYPE_NULL && typeof(b) > TYPE_NULL) {
		if (typeof(a) >= TYPE_STRING || typeof(b) >= TYPE_STRING) {
			//error
			return OBJECT_ERROR_TYPE;
		} else if (typeof(a) == TYPE_FLOAT || typeof(b) == TYPE_FLOAT) {
			out->type = TYPE_FLOAT;
			out->floating = _winter_castFloat(a) - _winter_castFloat(b);
			return OBJECT_OK;
		} else if (typeof(a) == TYPE_INT && typeof(b) == TYPE_INT) {
			out->type = TYPE_INT;
			out->integer = a->integer = b->integer;
			return OBJECT_OK;
		}
	}
	return OBJECT_ERROR_TYPE;
}
int _winter_objectMul(winterState_t *state, object_t *a, object_t *b) {
	object_t *out = a;
	a = deref(a);
	b = deref(b);
	if (typeof(a) > TYPE_NULL && typeof(b) > TYPE_NULL) {
		if (typeof(a) >= TYPE_STRING || typeof(b) >= TYPE_STRING) {
			//error
			return OBJECT_ERROR_TYPE;
		} else if (typeof(a) == TYPE_FLOAT || typeof(b) == TYPE_FLOAT) {
			out->type = TYPE_FLOAT;
			out->floating = _winter_castFloat(a) * _winter_castFloat(b);
			return OBJECT_OK;
		} else if (typeof(a) == TYPE_INT && typeof(b) == TYPE_INT) {
			out->type = TYPE_INT;
			out->integer = a->integer * b->integer;
			return OBJECT_OK;
		}
	}
	return OBJECT_ERROR_TYPE;
}
int _winter_objectDiv(winterState_t *state, object_t *a, object_t *b) {
	object_t *out = a;
	a = deref(a);
	b = deref(b);
	if (typeof(a) > TYPE_NULL && typeof(b) > TYPE_NULL) {
		if (typeof(a) >= TYPE_STRING || typeof(b) >= TYPE_STRING) {
			//error
			return OBJECT_ERROR_TYPE;
		} else if (typeof(a) == TYPE_FLOAT || typeof(b) == TYPE_FLOAT) {
			out->type = TYPE_FLOAT;
			out->floating = _winter_castFloat(a) / _winter_castFloat(b);
			return OBJECT_OK;
		} else if (typeof(a) == TYPE_INT && typeof(b) == TYPE_INT) {
			a->integer /= b->integer;
			return OBJECT_OK;
		}
	}
	return OBJECT_ERROR_TYPE;
}
int _winter_objectMod(winterState_t *state, object_t *a, object_t *b) {
	object_t *out = a;
	a = deref(a);
	b = deref(b);
	if (typeof(a) > TYPE_NULL && typeof(b) > TYPE_NULL) {
		if (typeof(a) >= TYPE_STRING || typeof(b) >= TYPE_STRING) {
			//error
			return OBJECT_ERROR_TYPE;
		} else if (typeof(a) == TYPE_FLOAT || typeof(b) == TYPE_FLOAT) {
			out->type = TYPE_FLOAT;
			out->floating = 0.0;
			return OBJECT_OK;
		} else if (typeof(a) == TYPE_INT && typeof(b) == TYPE_INT) {
			out->type = TYPE_INT;
			out->integer = a->integer % b->integer;
			return OBJECT_OK;
		}
	}
	return OBJECT_ERROR_TYPE;
}
int _winter_objectPow(winterState_t *state, object_t *a, object_t *b) {
	a = deref(a);
	b = deref(b);
	if (typeof(a) == TYPE_INT && typeof(b) == TYPE_INT) {
		a->integer = (winterInt_t)(pow(_winter_castFloat(a), _winter_castFloat(b)) + 0.5);
		return OBJECT_OK;
	} else if (isNumeric(a) && isNumeric(b)) {
		a->floating = (winterFloat_t)pow(_winter_castFloat(a), _winter_castFloat(b));
		a->type = TYPE_FLOAT;
		return OBJECT_OK;
	}
	return OBJECT_ERROR_TYPE;
}

int _winter_objectAssign(winterState_t *state, object_t *a, object_t *b) {
	if (a->type == TYPE_REFERENCE) {
		object_t *ptr = a->pointer;
		b = deref(b);
		object_t tempRef = *ptr;
		*ptr = *_winter_objectAddRef(state, b);
		*a   = *_winter_objectAddRef(state, ptr);
		_winter_objectDelRef(state, &tempRef);
		return OBJECT_OK;
	}
	return OBJECT_ERROR_TYPE;
}

int _winter_objectNegate(winterState_t *state, object_t *a) {
	a = deref(a);
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
