/*
** FILE: wparse.c
**
** PURPOSE: Functions for parsing tokens and generating ASTs
**
** Copyright (c) 2018 Zachary Wells, see LICENSE for more details
*/

#include "wparse.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define isExpression(t) ((t) == TK_IDENT || (t) == TK_INT || (t) == TK_FLOAT)
#define isOperator(t) ((t) >= TK_INC && (t) <= TK_NOT)

static int precedence(token_type_t operator) {
	switch(operator) {
		case TK_MUL:
		case TK_DIV:
		case TK_MOD:
			return 1;
		
		case TK_ADD:
		case TK_SUB:
			return 0;
		
		case TK_LPAREN:
		case TK_RPAREN:
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
	token_t token = {0};
	char *string = (char*)source;
	size_t forward = 0;
	
	token_type_t op_stack[8];
	int op_stack_size = 0;
	
	ast_node_t *expr_stack[16];
	int expr_stack_size = 0;
	
	enum {
		operator,
		expression
	} expect = expression;
	
	while (token.type != TK_EOF) {
		size_t tkSize = _winter_nextToken(string, &string, &token);
		
		switch (expect) {
			//expect operator
			case operator:
				if (isOperator(token.type)) {
					while (op_stack_size && precedence(op_stack[op_stack_size - 1]) >= precedence(token.type)) {
						ast_node_t *expr2 = expr_stack[--expr_stack_size];
						ast_node_t *expr1 = expr_stack[--expr_stack_size];
						
						token_t temp;
						temp.type = op_stack[--op_stack_size];
						ast_node_t *expr = createNode(&temp);
						
						expr->nodes[0] = expr1;
						expr->nodes[1] = expr2;
						expr_stack[expr_stack_size++] = expr;
					}
					op_stack[op_stack_size++] = token.type;
					expect = expression;
				} else {
					while (op_stack_size) {
						ast_node_t *expr2 = expr_stack[--expr_stack_size];
						ast_node_t *expr1 = expr_stack[--expr_stack_size];
						
						token_t temp;
						temp.type = op_stack[--op_stack_size];
						ast_node_t *expr = createNode(&temp);
						
						expr->nodes[0] = expr1;
						expr->nodes[1] = expr2;
						expr_stack[expr_stack_size++] = expr;
					}
					
					if (endPtr != NULL) {
						*endPtr = (char*)(source + forward);
					}
					return expr_stack[0];
				}
				break;
			//expect expression
			case expression:
				if (token.type == TK_LPAREN) {
					expr_stack[expr_stack_size++] = _winter_parseExpression(string, &string);
					_winter_nextToken(string, &string, &token);
					if (token.type != TK_RPAREN) {
						printf("expected closing parenthesis\n");
						exit(EXIT_FAILURE);
					}
					expect = operator;
				} else if (isExpression(token.type)) {
					expr_stack[expr_stack_size++] = createNode(&token);
					expect = operator;
				} else {
					//error, expression cant end like this
					printf("expected expression\n");
					exit(EXIT_FAILURE);
					return NULL;
				}
				break;
		}
		forward += tkSize;
	}
	
	//Should never reach here, unless I messed up.
	return NULL;
}

ast_node_t *_winter_parseStatement(const char *source) {
	char *string;
	return _winter_parseExpression(source, &string);
}

ast_node_t *generateTreeThing(const char *source) {
	return _winter_parseStatement(source);
}

ast_node_t *execute(ast_node_t *tree) {
	if (isOperator(tree->type)) {
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
			default: break;
		}
		free(branch1);
		free(branch2);
		tree->type = TK_INT;
	}
	return tree;
}
