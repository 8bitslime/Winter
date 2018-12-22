/*
** FILE: wobject.h
**
** PURPOSE: Objects in the Winter interpreter
**
** Copyright (c) 2018 Zachary Wells, see LICENSE for more details
*/

#include "wobject.h"

#define typeCheck(ta, tb) (a->type == (ta) && b->type == (tb))

int _winter_objectToInt(winterObject_t *dest, const winterObject_t *a) {
	switch (a->type) {
		case TYPE_INT:
			*dest = *a;
			return 1;
			
		case TYPE_FLOAT:
			dest->type = TYPE_FLOAT;
			dest->integer = (winterInt_t)a->floating;
			return 1;
		
		default: return 0;
	}
}

static winterFloat_t toFloat(const winterObject_t *a) {
	switch (a->type) {
		case TYPE_INT:
			return (winterFloat_t)a->integer;
		case TYPE_FLOAT:
			return a->floating;
		default:
			return 0;
	}
}

int _winter_objectAdd(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b) {
	if (typeCheck(TYPE_INT, TYPE_INT)) {
		dest->type = TYPE_INT;
		dest->integer = a->integer + b->integer;
		return 1;
	} else if (typeCheck(TYPE_FLOAT, TYPE_FLOAT)) {
		dest->type = TYPE_FLOAT;
		dest->floating = a->floating + b->floating;
		return 1;
	} else if (typeCheck(TYPE_FLOAT, TYPE_INT) || typeCheck(TYPE_INT, TYPE_FLOAT)) {
		dest->floating = toFloat(a) + toFloat(b);
		dest->type = TYPE_FLOAT;
		return 1;
	}
	
	return 0;
}

int _winter_objectSub(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b) {
	if (typeCheck(TYPE_INT, TYPE_INT)) {
		dest->type = TYPE_INT;
		dest->integer = a->integer - b->integer;
		return 1;
	} else if (typeCheck(TYPE_FLOAT, TYPE_FLOAT)) {
		dest->type = TYPE_FLOAT;
		dest->floating = a->floating - b->floating;
		return 1;
	} else if (typeCheck(TYPE_FLOAT, TYPE_INT) || typeCheck(TYPE_INT, TYPE_FLOAT)) {
		dest->floating = toFloat(a) - toFloat(b);
		dest->type = TYPE_FLOAT;
		return 1;
	}
	
	return 0;
}

int _winter_objectMul(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b) {
	if (typeCheck(TYPE_INT, TYPE_INT)) {
		dest->type = TYPE_INT;
		dest->integer = a->integer * b->integer;
		return 1;
	} else if (typeCheck(TYPE_FLOAT, TYPE_FLOAT)) {
		dest->type = TYPE_FLOAT;
		dest->floating = a->floating * b->floating;
		return 1;
	} else if (typeCheck(TYPE_FLOAT, TYPE_INT) || typeCheck(TYPE_INT, TYPE_FLOAT)) {
		dest->floating = toFloat(a) * toFloat(b);
		dest->type = TYPE_FLOAT;
		return 1;
	}
	
	return 0;
}

int _winter_objectDiv(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b) {
	if (typeCheck(TYPE_INT, TYPE_INT)) {
		dest->type = TYPE_INT;
		dest->integer = a->integer / b->integer;
		return 1;
	} else if (typeCheck(TYPE_FLOAT, TYPE_FLOAT)) {
		dest->type = TYPE_FLOAT;
		dest->floating = a->floating / b->floating;
		return 1;
	} else if (typeCheck(TYPE_FLOAT, TYPE_INT) || typeCheck(TYPE_INT, TYPE_FLOAT)) {
		dest->floating = toFloat(a) / toFloat(b);
		dest->type = TYPE_FLOAT;
		return 1;
	}
	
	return 0;
}

int _winter_objectMod(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b) {
	if (typeCheck(TYPE_INT, TYPE_INT)) {
		dest->type = TYPE_INT;
		dest->integer = a->integer % b->integer;
		return 1;
	} else if (a->type == TYPE_FLOAT || b->type == TYPE_FLOAT) {
		dest->type = TYPE_FLOAT;
		dest->floating = 0.0;
		return 1;
	}
	
	return 0;
}

int _winter_objectNegate(winterObject_t *dest, const winterObject_t *a) {
	if (a->type == TYPE_INT) {
		dest->type = TYPE_INT;
		dest->integer = -a->integer;
		return 1;
	} else if (a->type == TYPE_FLOAT) {
		dest->type = TYPE_FLOAT;
		dest->floating = -a->floating;
		return 1;
	}
	
	return 0;
}

int _winter_objectNot(winterObject_t *dest, const winterObject_t *a) {
	if (a->type == TYPE_INT || a->type == TYPE_FLOAT) {
		dest->type = TYPE_INT;
		dest->integer = !a->integer;
		return 1;
	}
	
	return 0;
}

int _winter_objectEqual(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b) {
	dest->type = TYPE_INT;
	dest->integer = a->integer == b->integer;
	return 1;
}

int _winter_objectAssign(winterObject_t *dest, winterObject_t *a, const winterObject_t *b) {
	//TODO: garbage collection
	*dest = *a = *b;
	return 1;
}

int _winter_objectPreInc(winterObject_t *dest, winterObject_t *a) {
	if (a->type == TYPE_INT) {
		dest->type = TYPE_INT;
		dest->integer = ++a->integer;
		return 1;
	} else if (a->type == TYPE_FLOAT) {
		dest->type = TYPE_FLOAT;
		dest->floating = ++a->floating;
		return 1;
	}
	
	return 0;
}

int _winter_objectPreDec(winterObject_t *dest, winterObject_t *a) {
	if (a->type == TYPE_INT) {
		dest->type = TYPE_INT;
		dest->integer = --a->integer;
		return 1;
	} else if (a->type == TYPE_FLOAT) {
		dest->type = TYPE_FLOAT;
		dest->floating = --a->floating;
		return 1;
	}
	
	return 0;
}
