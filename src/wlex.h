/*
** FILE: wlex.h
** 
** PURPOSE: Contains all functions necessary for splitting a string into tokens
**          and categorizing them.
**
** Copyright (c) 2018 Zachary Wells, see LICENSE for more details
*/

#ifndef WINLEX_H
#define WINLEX_H

#include "winter.h"
#include "wstate.h"
#include "wtable.h"
#include "wobject.h"

//All categorizations of tokens, order is specific, do not change please :)
typedef enum token_type_t {
	//Unexpected symbol
	TK_UNKNOWN = 0,
	
	//Identifier
	TK_IDENT,
	
	//Tokens for parsing only
	TK_VALUE, TK_NEGATE, TK_PRE_INC, TK_PRE_DEC,
	
	//Operators
	TK_INC, TK_DEC, TK_EXP,
	TK_MIN_EQ, TK_ADD_EQ, TK_MUL_EQ, TK_DIV_EQ,
	TK_EQ, TK_NEQ,
	TK_LEQ, TK_GEQ,
	TK_OR, TK_AND,
	TK_DOT, TK_ASSIGN, TK_LESS, TK_GREAT,
	TK_ADD, TK_SUB, TK_MUL, TK_DIV, TK_MOD,
	TK_BITOR, TK_BITAND, TK_NOT,
	
	//Literals
	TK_INT, TK_FLOAT, TK_STRING, TK_CHAR,
	
	//Keywords
	TK_FOR, TK_DO, TK_WHILE, TK_BREAK,
	TK_IF, TK_ELSE, TK_RETURN,
	
	//Symbols
	TK_COMMA, TK_SEMICOLON, TK_COLON,
	TK_LPAREN, TK_RPAREN,
	TK_LBRACKET, TK_RBRACKET,
	TK_LCURLY, TK_RCURLY,
	
	//End of file token
	TK_EOF
} token_type_t;

typedef struct token_t {
	token_type_t type;
	winterObject_t value;
	//TODO: debug info
} token_t;

typedef struct lexState_t {
	const char *source;
	size_t cur;  //cursor
	size_t line; //current line number
	token_t current; //most recent token
	//token_t lookahead
} lexState_t;

void _winter_lexStateInit(lexState_t *lexState, const char *source);
size_t _winter_nextToken(winterState_t *state, lexState_t *lexState);

#endif
