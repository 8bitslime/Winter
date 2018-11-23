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

//All categorizations of tokens, order is specific to not change please :)
typedef enum token_type_t {
	//Unexpected symbol
	TK_UNKNOWN = 0,
	
	//Keywords
	TK_FOR, TK_DO, TK_WHILE, TK_BREAK,
	TK_IF, TK_ELSE, TK_RETURN,
	
	//Symbols
	TK_COMMA, TK_SEMICOLON,
	TK_LPAREN, TK_RPAREN,
	TK_LBRACKET, TK_RBRACKET,
	TK_LCURLY, TK_RCURLY,
	
	//Operators
	TK_INC, TK_DEC,
	TK_MIN_EQ, TK_ADD_EQ, TK_EQ, TK_NEQ,
	TK_LEQ, TK_GEQ,
	TK_OR, TK_AND,
	TK_ASSIGN, TK_LESS, TK_GREAT,
	TK_ADD, TK_SUB, TK_MUL, TK_DIV, TK_MOD,
	TK_BITOR, TK_BITAND, TK_NOT,
	
	//Identifier
	TK_IDENT,
	
	//Literals
	TK_INT, TK_FLOAT, TK_STRING, TK_CHAR
} token_type_t;

typedef union token_info_t {
	int integer;
	
} token_info_t;

#endif
