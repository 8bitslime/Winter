#ifndef WINTER_LEXER_H
#define WINTER_LEXER_H

#include "wtype.h"

typedef enum token_type_t {
	TK_UNKNOWN = 0,
	
	//End of source file/text
	TK_EOF,
	
	//Identifier and literals
	TK_IDENT,
	TK_DECIMAL, TK_HEX, TK_BINARY, TK_OCTAL,
	TK_CHAR, TK_FLOAT, TK_STRING,
	
	//Keywords
	TK_FOR, TK_DO, TK_WHILE, TK_IF, TK_ELSE, TK_LET,
	
	//Operators
	TK_LSHIFTEQ, TK_RSHIFTEQ,
	TK_INC, TK_DEC, TK_POW,
	TK_ADDEQ, TK_SUBEQ, TK_MULEQ, TK_DIVEQ, TK_MODEQ,
	TK_OR, TK_AND,
	TK_OREQ, TK_ANDEQ, TK_XOREQ,
	TK_LSHIFT, TK_RSHIFT,
	TK_NOTEQ, TK_LEQ, TK_GEQ, TK_EQ,
	TK_DOT, TK_COMMA, TK_ASSIGN,
	TK_ADD, TK_SUB,
	TK_MUL, TK_DIV, TK_MOD,
	TK_LESS, TK_GREATER,
	TK_BITAND, TK_BITOR, TK_NOT,
	TK_XOR, TK_BITNOT,
	
	//Symbols
	TK_LPAREN, TK_RPAREN,
	TK_LBRACKET, TK_RBRACKET,
	TK_LCURLY, TK_RCURLY,
	TK_SEMICOLON,
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
