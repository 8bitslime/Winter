#ifndef PARSER_H
#define PARSER_H

#include "winter.h"
#include "lexer.h"
#include <stddef.h>

#define isOperator(t) (((token_type_t)(t) >= TK_LSHIFTEQ && (token_type_t)(t) <= TK_EQ)   || \
					  ((t) == '=' || (t) == '+' || (t) == '-' || \
					  (t) == '*' || (t) == '/' || (t) == '%'  || \
					  (t) == '<' || (t) == '>' || (t) == '&'  || \
					  (t) == '|' || (t) == '!' || (t) == '^'  || \
					  (t) == '~') || \
					  (ast_node_type_t)(t) == AST_NEGATE || (ast_node_type_t)(t) == AST_PASS)
					  
#define isUnarySymbol(t)   ((t) == '-' || (t) == '!')
#define isUnary(t) ((t) == AST_NOT || (t) == AST_NEGATE)

#define isExpression(t) ((t) == '(' || ((token_type_t)(t) >= TK_IDENT && (token_type_t)(t) <= TK_STRING)) 

typedef enum ast_node_type_t {
	AST_ASSIGN = '=',
	AST_ADD = '+', AST_SUB = '-',
	AST_MUL = '*', AST_DIV = '/', AST_MOD = '%',
	AST_LESS = '<', AST_GREATER = '>',
	AST_BITAND = '&', AST_BITOR = '|', AST_NOT = '!',
	AST_BITXOR = '^', AST_BITNOT = '~',
	
	//Simply passes the value through, needed for some precedence
	AST_PASS,
	
	AST_IDENT = TK_IDENT,
	AST_VALUE,
	
	AST_FOR = TK_FOR,
	
	AST_LSHIFTEQ = TK_LSHIFTEQ, AST_RSHIFTEQ = TK_RSHIFTEQ,
	// TK_INC, TK_DEC,
	AST_POW = TK_POW,
	AST_ADDEQ = TK_ADDEQ, AST_SUBEQ = TK_SUBEQ,
	AST_MULEQ = TK_MULEQ, AST_DIVEQ = TK_DIVEQ, AST_MODEQ = TK_MODEQ,
	AST_OR = TK_OR, AST_AND = TK_AND,
	AST_OREQ = TK_OREQ, AST_ANDew = TK_ANDEQ, AST_XOREQ = TK_XOREQ,
	AST_LSHIFT = TK_LSHIFT, AST_RSHIFT = TK_RSHIFT,
	AST_NOTEQ = TK_NOTEQ, AST_LEQ = TK_LEQ, AST_GEQ = TK_GEQ, AST_EQ = TK_EQ,
	
	AST_NEGATE
} ast_node_type_t;

typedef struct ast_node_t {
	ast_node_type_t type;
	int value;
	
	size_t numNodes;
	struct ast_node_t **children;
} ast_node_t;

ast_node_t *_winter_generateTree(winterState_t *state, const char *source);

#endif
