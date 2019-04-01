#ifndef WINTER_LEXER_H
#define WINTER_LEXER_H

#include "winter.h"

typedef enum token_type_t {
	TK_UNKNOWN = 0,
	//Single character symbols just use their ascii value
	
	//End of source file/text
	TK_EOF = 128,
	
	//Identifier and literals
	TK_IDENT, TK_DECIMAL, TK_HEX, TK_BINARY, TK_OCTAL, TK_CHAR, TK_FLOAT, TK_STRING,
	
	//Keywords
	TK_FOR, TK_DO, TK_WHILE, TK_IF, TK_ELSE,
	
	//Operators
	TK_LSHIFTEQ, TK_RSHIFTEQ,
	TK_INC, TK_DEC, TK_POW,
	TK_ADDEQ, TK_SUBEQ, TK_MULEQ, TK_DIVEQ, TK_MODEQ,
	TK_OR, TK_AND,
	TK_OREQ, TK_ANDEQ, TK_XOREQ,
	TK_LSHIFT, TK_RSHIFT,
	TK_NOTEQ, TK_LEQ, TK_GEQ, TK_EQ
} token_type_t;

typedef struct cursor_t {
	size_t pos, carrot, line;
	const char *pointer;
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

//returns number of characters parsed
//returns 0 if end of source and sets current token to EOF
int _winter_lexNext(lexState_t *lex);

#endif
