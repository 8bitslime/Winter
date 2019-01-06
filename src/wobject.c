/*
** FILE: wobject.h
**
** PURPOSE: Objects in the Winter interpreter
**
** Copyright (c) 2019 Zachary Wells, see LICENSE for more details
*/

#include "wobject.h"
#include "wtable.h"

#include <math.h>

#define isRefCounted(t) ((t) == TYPE_TABLE || (t) == TYPE_STRING)

winterObject_t *_winter_objectAddRef(winterState_t *state, winterObject_t *object) {
	if (isRefCounted(object->type)) {
		refcounted_t *obj = object->pointer;
		obj->refcount++;
	}
	return object;
}

winterObject_t *_winter_objectDelRef(winterState_t *state, winterObject_t *object) {
	if (isRefCounted(object->type)) {
		refcounted_t *obj = object->pointer;
		obj->refcount--;
		if (obj->refcount <= 0) {
			switch (object->type) {
				case TYPE_TABLE:
					_winter_tableFree(state, object->pointer);
					break;
				case TYPE_STRING:
					_winter_stringFree(state, object->string);
					break;
				
				default: break;
			}
		}
	}
	return object;
}

#define typeCheck(ta, tb) (a->type == (ta) && b->type == (tb))

bool_t _winter_objectToInt(winterState_t *state, winterObject_t *dest, const winterObject_t *a) {
	switch (a->type) {
		case TYPE_INT:
			*dest = *a;
			return true;
			
		case TYPE_FLOAT:
			dest->type = TYPE_FLOAT;
			dest->integer = (winterInt_t)a->floating;
			return true;
		
		default: return false;
	}
}

static winterFloat_t toFloat(const winterObject_t *a) {
	switch (a->type) {
		case TYPE_INT:
			return (winterFloat_t)a->integer;
		case TYPE_FLOAT:
			return a->floating;
			
		default: return 0.0;
	}
}

bool_t _winter_objectAdd(winterState_t *state, winterObject_t *dest, const winterObject_t *a, const winterObject_t *b) {
	if (typeCheck(TYPE_INT, TYPE_INT)) {
		dest->type = TYPE_INT;
		dest->integer = a->integer + b->integer;
		return true;
	} else if (typeCheck(TYPE_FLOAT, TYPE_FLOAT)) {
		dest->type = TYPE_FLOAT;
		dest->floating = a->floating + b->floating;
		return true;
	} else if (typeCheck(TYPE_FLOAT, TYPE_INT) || typeCheck(TYPE_INT, TYPE_FLOAT)) {
		dest->floating = toFloat(a) + toFloat(b);
		dest->type = TYPE_FLOAT;
		return true;
	}
	
	return false;
}

bool_t _winter_objectSub(winterState_t *state, winterObject_t *dest, const winterObject_t *a, const winterObject_t *b) {
	if (typeCheck(TYPE_INT, TYPE_INT)) {
		dest->type = TYPE_INT;
		dest->integer = a->integer - b->integer;
		return true;
	} else if (typeCheck(TYPE_FLOAT, TYPE_FLOAT)) {
		dest->type = TYPE_FLOAT;
		dest->floating = a->floating - b->floating;
		return true;
	} else if (typeCheck(TYPE_FLOAT, TYPE_INT) || typeCheck(TYPE_INT, TYPE_FLOAT)) {
		dest->floating = toFloat(a) - toFloat(b);
		dest->type = TYPE_FLOAT;
		return true;
	}
	
	return false;
}

bool_t _winter_objectMul(winterState_t *state, winterObject_t *dest, const winterObject_t *a, const winterObject_t *b) {
	if (typeCheck(TYPE_INT, TYPE_INT)) {
		dest->type = TYPE_INT;
		dest->integer = a->integer * b->integer;
		return true;
	} else if (typeCheck(TYPE_FLOAT, TYPE_FLOAT)) {
		dest->type = TYPE_FLOAT;
		dest->floating = a->floating * b->floating;
		return true;
	} else if (typeCheck(TYPE_FLOAT, TYPE_INT) || typeCheck(TYPE_INT, TYPE_FLOAT)) {
		dest->floating = toFloat(a) * toFloat(b);
		dest->type = TYPE_FLOAT;
		return true;
	}
	
	return false;
}

bool_t _winter_objectDiv(winterState_t *state, winterObject_t *dest, const winterObject_t *a, const winterObject_t *b) {
	if (typeCheck(TYPE_INT, TYPE_INT)) {
		dest->type = TYPE_INT;
		dest->integer = a->integer / b->integer;
		return true;
	} else if (typeCheck(TYPE_FLOAT, TYPE_FLOAT)) {
		dest->type = TYPE_FLOAT;
		dest->floating = a->floating / b->floating;
		return true;
	} else if (typeCheck(TYPE_FLOAT, TYPE_INT) || typeCheck(TYPE_INT, TYPE_FLOAT)) {
		dest->floating = toFloat(a) / toFloat(b);
		dest->type = TYPE_FLOAT;
		return true;
	}
	
	return false;
}

bool_t _winter_objectMod(winterState_t *state, winterObject_t *dest, const winterObject_t *a, const winterObject_t *b) {
	if (typeCheck(TYPE_INT, TYPE_INT)) {
		dest->type = TYPE_INT;
		dest->integer = a->integer % b->integer;
		return true;
	} else if (a->type == TYPE_FLOAT || b->type == TYPE_FLOAT) {
		dest->type = TYPE_FLOAT;
		dest->floating = 0.0;
		return true;
	}
	
	return false;
}

bool_t _winter_objectPow(winterState_t *state, winterObject_t *dest, const winterObject_t *a, const winterObject_t *b) {
	if (typeCheck(TYPE_INT, TYPE_INT)) {
		dest->type = TYPE_INT;
		dest->integer = (winterInt_t)(pow(toFloat(a), toFloat(b)) + 0.5);
		return true;
	} else if (a->type == TYPE_FLOAT || b->type == TYPE_FLOAT) {
		dest->type = TYPE_FLOAT;
		dest->floating = pow(toFloat(a), toFloat(b));
		return true;
	}
	
	return false;
}

bool_t _winter_objectNegate(winterState_t *state, winterObject_t *dest, const winterObject_t *a) {
	if (a->type == TYPE_INT) {
		dest->type = TYPE_INT;
		dest->integer = -a->integer;
		return true;
	} else if (a->type == TYPE_FLOAT) {
		dest->type = TYPE_FLOAT;
		dest->floating = -a->floating;
		return true;
	}
	
	return false;
}

bool_t _winter_objectNot(winterState_t *state, winterObject_t *dest, const winterObject_t *a) {
	if (a->type == TYPE_INT || a->type == TYPE_FLOAT) {
		dest->type = TYPE_INT;
		dest->integer = !a->integer;
		return true;
	}
	
	return false;
}

bool_t _winter_objectEqual(winterState_t *state, winterObject_t *dest, const winterObject_t *a, const winterObject_t *b) {
	dest->type = TYPE_INT;
	dest->integer = a->integer == b->integer;
	return true;
}

bool_t _winter_objectAssign(winterState_t *state, winterObject_t *dest, winterObject_t *a, const winterObject_t *b) {
	*dest = *_winter_objectDelRef(state, a) = *_winter_objectAddRef(state, b);
	return true;
}

bool_t _winter_objectAddEq(winterState_t *state, winterObject_t *dest, winterObject_t *a, const winterObject_t *b) {
	bool_t result = _winter_objectAdd(state, a, a, b);
	if (result) {
		*dest = *a;
		return true;
	}
	return false;
}

bool_t _winter_objectMinEq(winterState_t *state, winterObject_t *dest, winterObject_t *a, const winterObject_t *b) {
	bool_t result = _winter_objectSub(state, a, a, b);
	if (result) {
		*dest = *a;
		return true;
	}
	return false;
}

bool_t _winter_objectMulEq(winterState_t *state, winterObject_t *dest, winterObject_t *a, const winterObject_t *b) {
	bool_t result = _winter_objectMul(state, a, a, b);
	if (result) {
		*dest = *a;
		return true;
	}
	return false;
}

bool_t _winter_objectDivEq(winterState_t *state, winterObject_t *dest, winterObject_t *a, const winterObject_t *b) {
	bool_t result = _winter_objectDiv(state, a, a, b);
	if (result) {
		*dest = *a;
		return true;
	}
	return false;
}

bool_t _winter_objectPreInc(winterState_t *state, winterObject_t *dest, winterObject_t *a) {
	if (a->type == TYPE_INT) {
		dest->type = TYPE_INT;
		dest->integer = ++a->integer;
		return true;
	} else if (a->type == TYPE_FLOAT) {
		dest->type = TYPE_FLOAT;
		dest->floating = ++a->floating;
		return true;
	}
	
	return false;
}

bool_t _winter_objectPreDec(winterState_t *state, winterObject_t *dest, winterObject_t *a) {
	if (a->type == TYPE_INT) {
		dest->type = TYPE_INT;
		dest->integer = --a->integer;
		return true;
	} else if (a->type == TYPE_FLOAT) {
		dest->type = TYPE_FLOAT;
		dest->floating = --a->floating;
		return true;
	}
	
	return false;
}
