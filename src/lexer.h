#ifndef WINTER_LEXER_H
#define WINTER_LEXER_H

#include "winter.h"

typedef enum token_type_t {
	TK_UNKNOWN = 0,
	TK_EOF = 128,
	TK_IDENT,
	TK_FOR
} token_type_t;

typedef struct cursor_t {
	size_t pos, carrot, line;
} cursor_t;

typedef struct token_t {
	token_type_t type;
	cursor_t cursor;
	size_t size;
} token_t;

typedef struct lexState_t {
	const char *string;
	cursor_t cursor;
	token_t current;
	token_t lookahead;
} lexState_t;

int _winter_lexNext(lexState_t *lex);

#endif
