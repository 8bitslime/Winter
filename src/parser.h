#ifndef PARSER_H
#define PARSER_H

#include "winter.h"
#include "lexer.h"
#include "object.h"
#include <stddef.h>

#define isOperator(t) (((token_type_t)(t) >= TK_LSHIFTEQ && (token_type_t)(t) <= TK_BITNOT) || \
					  (ast_node_type_t)(t) == AST_NEGATE || (ast_node_type_t)(t) == AST_PASS)

#define isUnarySymbol(t) ((t) == TK_SUB || (t) == TK_NOT)
#define isUnary(t)       ((t) == AST_NOT || (t) == AST_NEGATE)

#define isExpression(t) ((token_type_t)(t) == TK_LPAREN || ((token_type_t)(t) >= TK_IDENT && (token_type_t)(t) <= TK_STRING))

typedef enum ast_node_type_t {
	AST_UNKNOWN = 0,
	
	//Simply passes the value through, needed for some precedence
	AST_PASS,
	
	AST_IDENT = TK_IDENT,
	AST_VALUE,
	
	//Keywords to be used in the AST
	AST_FOR = TK_FOR,
	
	//Operators, just coppied over except for a few semantics
	AST_LSHIFTEQ = TK_LSHIFTEQ, AST_RSHIFTEQ = TK_RSHIFTEQ,
	//TODO: post and pre variants
	// TK_INC, TK_DEC,
	AST_POW = TK_POW,
	AST_ADDEQ = TK_ADDEQ, AST_SUBEQ = TK_SUBEQ,
	AST_MULEQ = TK_MULEQ, AST_DIVEQ = TK_DIVEQ, AST_MODEQ = TK_MODEQ,
	AST_OR = TK_OR, AST_AND = TK_AND,
	AST_OREQ = TK_OREQ, AST_ANDEQ = TK_ANDEQ, AST_XOREQ = TK_XOREQ,
	AST_LSHIFT = TK_LSHIFT, AST_RSHIFT = TK_RSHIFT,
	AST_NOTEQ = TK_NOTEQ, AST_LEQ = TK_LEQ, AST_GEQ = TK_GEQ, AST_EQ = TK_EQ,
	AST_DOT = TK_DOT, AST_COMMA = TK_COMMA, AST_ASSIGN = TK_ASSIGN,
	AST_ADD = TK_ADD, AST_SUB = TK_SUB,
	AST_MUL = TK_MUL, AST_DIV = TK_DIV, AST_MOD = TK_MOD,
	AST_LESS = TK_LESS, AST_GREATER = TK_GREATER,
	AST_BITAND = TK_BITAND, AST_BITOR = TK_BITOR, AST_NOT = TK_NOT,
	AST_XOR = TK_XOR, AST_BITNOT = TK_BITNOT,
	
	AST_NEGATE
} ast_node_type_t;

typedef struct ast_node_t {
	ast_node_type_t type;
	object_t value;
	
	size_t numNodes;
	struct ast_node_t **children;
} ast_node_t;

ast_node_t *_winter_generateTree(winterState_t *state, const char *source);

#endif
