/*
** FILE: wobject.h
**
** PURPOSE: Objects in the Winter interpreter
**
** Copyright (c) 2018 Zachary Wells, see LICENSE for more details
*/

#include "wobject.h"

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

int _winter_objectAdd(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b) {
	switch (a->type) {
		case TYPE_INT: {
			if (b->type == TYPE_INT) {
				dest->type = TYPE_INT;
				dest->integer = a->integer + b->integer;
				return 1;
			} else if (b->type == TYPE_FLOAT) {
				dest->type = TYPE_FLOAT;
				dest->floating = (winterFloat_t)a->integer + b->floating;
				return 1;
			}
			return 0;
		} break;
		
		case TYPE_FLOAT: {
			if (b->type == TYPE_FLOAT) {
				dest->type = TYPE_FLOAT;
				dest->floating = a->floating + b->floating;
				return 1;
			} else if (b->type == TYPE_INT) {
				dest->type = TYPE_FLOAT;
				dest->integer = (winterInt_t)a->floating + b->integer;
				return 1;
			}
			return 0;
		} break;
		
		default: break;
	}
	return 0;
}

int _winter_objectSub(winterObject_t *dest, const winterObject_t *a, const winterObject_t *b) {
	switch (a->type) {
		case TYPE_INT: {
			if (b->type == TYPE_INT) {
				dest->type = TYPE_INT;
				dest->integer = a->integer - b->integer;
				return 1;
			} else if (b->type == TYPE_FLOAT) {
				dest->type = TYPE_FLOAT;
				dest->floating = (winterFloat_t)a->integer - b->floating;
				return 1;
			}
			return 0;
		} break;
		
		case TYPE_FLOAT: {
			if (b->type == TYPE_FLOAT) {
				dest->type = TYPE_FLOAT;
				dest->floating = a->floating - b->floating;
				return 1;
			} else if (b->type == TYPE_INT) {
				dest->type = TYPE_FLOAT;
				dest->floating = a->floating - (winterFloat_t)b->integer;
				return 1;
			}
			return 0;
		} break;
		
		default: break;
	}
	return 0;
}
