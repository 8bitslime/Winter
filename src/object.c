#include "object.h"
#include "wtype.h"

void _winter_tokenToObject(winterState_t *state, const token_t *token, object_t *dest) {
	dest->type = TYPE_INT;
	switch (token->type) {
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
	switch (object->type)  {
		case TYPE_INT:
			return object->integer;
		
		case TYPE_FLOAT:
			return (winterInt_t)object->floating;
		
		default: return 0;
	}
}

winterFloat_t _winter_castFloat(const object_t *object) {
	switch (object->type)  {
		case TYPE_INT:
			return (winterFloat_t)object->integer;
		
		case TYPE_FLOAT:
			return object->floating;
		
		default: return 0;
	}
}
