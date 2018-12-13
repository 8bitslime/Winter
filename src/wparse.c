/*
** FILE: wparse.c
**
** PURPOSE: Functions for parsing tokens and generating ASTs
**
** Copyright (c) 2018 Zachary Wells, see LICENSE for more details
*/

#include "wparse.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define isExpression(t) ((t) == TK_IDENT || (t) == TK_INT || (t) == TK_FLOAT || (t) == TK_LPAREN)
#define isOperator(t) ((t) >= TK_INC && (t) <= TK_NOT)

static int precedence(token_type_t operator) {
	switch(operator) {
		case TK_INT:
			return 69;
		
		case TK_MUL:
		case TK_DIV:
		case TK_MOD:
			return 1;
		
		case TK_ADD:
		case TK_SUB:
			return 0;
		
		default:
			return -1;
	}
}

static ast_node_t *allocNode(size_t numNodes) {
	//TODO: better allocator
	ast_node_t *ret = malloc(sizeof(ast_node_t) + sizeof(ast_node_t*) * numNodes);
	ret->numNodes = numNodes;
	ret->nodes = (ast_node_t**)(ret + 1);
	return ret;
}

static ast_node_t *createNode(const token_t *token) {
	ast_node_t *ret = NULL;
	
	if (isExpression(token->type)) {
		ret = allocNode(0);
		switch (token->type) {
			case TK_INT:
				ret->value.integer = token->value.integer;
				break;
			case TK_FLOAT:
				ret->value.floating = token->value.floating;
				break;
			default: break;
		}
	} else if (isOperator(token->type)) {
		ret = allocNode(2);
	}
	
	ret->type = token->type;
	return ret;
}

ast_node_t *_winter_parseExpression(const char *source, char **endPtr) {
	char *string = (char*)source;
	ast_node_t *top, *append = NULL, *parenthesis = NULL;
	token_t token = {0};
	
	enum {
		expression,
		operator
	} expect = expression;
	
	while (token.type != TK_EOF) {
		size_t forward = _winter_nextToken(string, &string, &token);
		
		switch (expect) {
			case expression: {
				if (isExpression(token.type)) {
					ast_node_t *node;
					
					if (token.type == TK_LPAREN) {
						node = parenthesis = _winter_parseExpression(string, &string);
						_winter_nextToken(string, &string, &token);
						
						if (token.type != TK_RPAREN) {
							return NULL;
						}
					} else {
						node = createNode(&token);
					}
					
					if (append) {
						append->nodes[1] = node;
					} else {
						top = node;
					}
					expect = operator;
				} else {
					return NULL;
				}
			} break;
			
			case operator: {
				if (isOperator(token.type)) {
					ast_node_t *node = createNode(&token);
					
					if (top == parenthesis || precedence(top->type) >= precedence(token.type)) {
						node->nodes[0] = top;
						top = append = node;
					} else {
						ast_node_t *current = top;
						while (current != parenthesis && precedence(current->nodes[1]->type) < precedence(token.type)) {
							current = current->nodes[1];
						}
						node->nodes[0] = current->nodes[1];
						current->nodes[1] = node;
						append = node;
					}
					
					expect = expression;
				} else {
					*endPtr = string - forward;
					return top;
				}
			} break;
		}
	}
	return top;
}

ast_node_t *_winter_parseStatement(const char *source, char **endPtr) {
	return _winter_parseExpression(source, endPtr);
}

ast_node_t *generateTreeThing(winterState_t *state, const char *source) {
	char *string;
	return _winter_parseStatement(source, &string);
}

ast_node_t *execute(ast_node_t *tree) {
	if (tree && isOperator(tree->type)) {
		ast_node_t *branch1 = execute(tree->nodes[0]);
		ast_node_t *branch2 = execute(tree->nodes[1]);
		switch (tree->type) {
			case TK_ADD:
				tree->value.integer = branch1->value.integer + branch2->value.integer;
				break;
			case TK_SUB:
				tree->value.integer = branch1->value.integer - branch2->value.integer;
				break;
			case TK_MUL:
				tree->value.integer = branch1->value.integer * branch2->value.integer;
				break;
			case TK_DIV:
				tree->value.integer = branch1->value.integer / branch2->value.integer;
				break;
			case TK_MOD:
				tree->value.integer = branch1->value.integer % branch2->value.integer;
				break;
			default: break;
		}
		free(branch1);
		free(branch2);
		tree->type = TK_INT;
		tree->numNodes = 0;
	}
	return tree;
}
