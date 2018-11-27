/*
** FILE: wparser.c
**
** PURPOSE: Parses tokens and creates and AST
**
** Copyright (c) 2018 Zachary Wells, see LICENSE for more details
*/

#ifndef WPARSER_H
#define WPARSER_H

#include "wparser.h"
#include "wlex.h"

#include <assert.h>
#include <stdlib.h>

static ast_node_t *nodeAlloc(size_t numChildren) {
	ast_node_t *ret = malloc(sizeof(ast_node_t) + sizeof(ast_node_t*) * numChildren);
	ret->numChildren = numChildren;
	ret->children = (ast_node_t**)(ret + 1);
	return ret;
}

/*
TK_COMMA, TK_SEMICOLON, TK_COLON,
TK_LPAREN, TK_RPAREN,
TK_LBRACKET, TK_RBRACKET,
TK_LCURLY, TK_RCURLY,
TK_INC, TK_DEC,
TK_MIN_EQ, TK_ADD_EQ, TK_EQ, TK_NEQ,
TK_LEQ, TK_GEQ,
TK_OR, TK_AND,
TK_DOT, TK_ASSIGN, TK_LESS, TK_GREAT,
TK_ADD, TK_SUB, TK_MUL, TK_DIV, TK_MOD,
TK_BITOR, TK_BITAND, TK_NOT,
*/

static const int precedence[] = {
	0, 0, 0,
	0, 0,
	0, 0,
	0, 0,
	0, 0,
	0, 0, 0,0,
	0, 0,
	0, 0,
	0, 0, 0, 0,
	2, 2, 3, 3, 3,
	0, 0, 0
};

ast_node_t *_winter_parseExpression(const char *source, char **endPtr) {
	token_t current;
	
	token_type_t operators[20];
	int operator_size = 0;
	
	ast_node_t *expressions[20];
	int expression_size = 0;
	
	char *string = (char*)source;
	while (_winter_nextToken(string, &string, &current)) {
		if (current.type == TK_LPAREN) {
			operators[operator_size++] = current.type;
			
		} else if (current.type == TK_INT) {
			
			ast_node_t *node = nodeAlloc(0);
			node->type = AST_INT;
			node->info.integer = current.info.integer;
			expressions[expression_size++] = node;
			
		} else if (isTKOperator(current.type)) {
			if (operator_size) {
				int cur = precedence[current.type - TK_COMMA];
				while (operator_size && precedence[operators[operator_size-1] - TK_COMMA] >= cur) {
					token_type_t operator = operators[--operator_size];
					ast_node_t *e1 = expressions[--expression_size];
					ast_node_t *e0 = expressions[--expression_size];
					
					ast_node_t *node = nodeAlloc(2);
					node->type = (operator - TK_INC) + AST_INC;
					node->children[0] = e0;
					node->children[1] = e1;
					
					expressions[expression_size++] = node;
				}
			}
			
			operators[operator_size++] = current.type;
			
		} else if (current.type == TK_RPAREN) {
			while (operators[operator_size-1] != TK_LPAREN) {
				token_type_t operator = operators[--operator_size];
				ast_node_t *e1 = expressions[--expression_size];
				ast_node_t *e0 = expressions[--expression_size];
				
				ast_node_t *node = nodeAlloc(2);
				node->type = (operator - TK_INC) + AST_INC;
				node->children[0] = e0;
				node->children[1] = e1;
				
				expressions[expression_size++] = node;
			}
			
			operator_size--;
		}
	}
	
	while (operator_size > 0) {
		token_type_t operator = operators[--operator_size];
		ast_node_t *e1 = expressions[--expression_size];
		ast_node_t *e0 = expressions[--expression_size];
		
		ast_node_t *node = nodeAlloc(2);
		node->type = (operator - TK_INC) + AST_INC;
		node->children[0] = e0;
		node->children[1] = e1;
		
		expressions[expression_size++] = node;
	}
	
	return expressions[expression_size-1];
}

ast_node_t *_winter_executeAST(ast_node_t *tree) {
	if (tree->type != AST_INT) {
		ast_node_t *left = _winter_executeAST(tree->children[0]);
		ast_node_t *right = _winter_executeAST(tree->children[1]);
		switch (tree->type) {
			case AST_ADD:
				tree->info.integer = left->info.integer + right->info.integer;
				break;
			case AST_SUB:
				tree->info.integer = left->info.integer - right->info.integer;
				break;
			case AST_MUL:
				tree->info.integer = left->info.integer * right->info.integer;
				break;
			case AST_DIV:
				tree->info.integer = left->info.integer / right->info.integer;
				break;
			
			default: assert(0);
		}
		free(left);
		free(right);
		tree->type = AST_INT;
	}
	return tree;
}

#endif
