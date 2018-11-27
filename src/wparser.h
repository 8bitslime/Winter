/*
** FILE: wparser.h
**
** PURPOSE: Parses tokens and creates and AST
**
** Copyright (c) 2018 Zachary Wells, see LICENSE for more details
*/

#include "winter.h"

typedef enum ast_type_t {
	AST_UNKNOWN = 0,
	
	AST_INC, AST_DEC,
	AST_MIN_EQ, AST_ADD_EQ, AST_EQ, AST_NEQ,
	AST_LEQ, AST_GEQ,
	AST_OR, AST_AND,
	AST_DOT, AST_ASSIGN, AST_LESS, AST_GREAT,
	AST_ADD, AST_SUB, AST_MUL, AST_DIV, AST_MOD,
	AST_BITOR, AST_BITAND, AST_NOT,
	
	AST_IDENT,
	
	AST_INT, AST_FLOAT, AST_STRING,
} ast_type_t;

typedef struct ast_node_t {
	union {
		winterInt_t integer;
		winterInt_t floating;
	} info;
	ast_type_t type;
	
	size_t numChildren;
	struct ast_node_t **children;
} ast_node_t;

ast_node_t *_winter_parseExpression(const char *source, char **endPtr);
ast_node_t *_winter_executeAST(ast_node_t *tree);
