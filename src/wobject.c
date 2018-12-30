/*
** FILE: wobject.h
**
** PURPOSE: Objects in the Winter interpreter
**
** Copyright (c) 2018 Zachary Wells, see LICENSE for more details
*/

#include "wobject.h"
#include "math.h"

#define typeCheck(ta, tb) (a->type == (ta) && b->type == (tb))

bool_t _winter_objectToInt(winterObject_t *dest, const winterObject_t *a) {
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

bool_t _winter_objectAdd(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b) {
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

bool_t _winter_objectSub(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b) {
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

bool_t _winter_objectMul(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b) {
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

bool_t _winter_objectDiv(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b) {
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

bool_t _winter_objectMod(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b) {
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

bool_t _winter_objectPow(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b) {
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

bool_t _winter_objectNegate(winterObject_t *dest, const winterObject_t *a) {
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

bool_t _winter_objectNot(winterObject_t *dest, const winterObject_t *a) {
	if (a->type == TYPE_INT || a->type == TYPE_FLOAT) {
		dest->type = TYPE_INT;
		dest->integer = !a->integer;
		return true;
	}
	
	return false;
}

bool_t _winter_objectEqual(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b) {
	dest->type = TYPE_INT;
	dest->integer = a->integer == b->integer;
	return true;
}

bool_t _winter_objectAssign(winterObject_t *dest, winterObject_t *a, const winterObject_t *b) {
	//TODO: reference counting
	*dest = *a = *b;
	return true;
}

bool_t _winter_objectAddEq(winterObject_t *dest, winterObject_t *a, const winterObject_t *b) {
	bool_t result = _winter_objectAdd(a, a, b);
	if (result) {
		*dest = *a;
		return true;
	}
	return false;
}

bool_t _winter_objectMinEq(winterObject_t *dest, winterObject_t *a, const winterObject_t *b) {
	bool_t result = _winter_objectSub(a, a, b);
	if (result) {
		*dest = *a;
		return true;
	}
	return false;
}

bool_t _winter_objectMulEq(winterObject_t *dest, winterObject_t *a, const winterObject_t *b) {
	bool_t result = _winter_objectMul(a, a, b);
	if (result) {
		*dest = *a;
		return true;
	}
	return false;
}

bool_t _winter_objectDivEq(winterObject_t *dest, winterObject_t *a, const winterObject_t *b) {
	bool_t result = _winter_objectDiv(a, a, b);
	if (result) {
		*dest = *a;
		return true;
	}
	return false;
}

bool_t _winter_objectPreInc(winterObject_t *dest, winterObject_t *a) {
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

bool_t _winter_objectPreDec(winterObject_t *dest, winterObject_t *a) {
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
