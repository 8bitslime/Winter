/*
** FILE: wparse.c
**
** PURPOSE: Functions for parsing tokens and generating ASTs
**
** Copyright (c) 2018 Zachary Wells, see LICENSE for more details
*/

#include "wparse.h"
#include "wstate.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define isExpression(t) ((t) == TK_IDENT || (t) == TK_VALUE || (t) == TK_LPAREN)
#define isUnary(t)      ((t) == TK_NOT || (t) == TK_NEGATE || (t) == TK_PRE_INC || (t) == TK_PRE_DEC)
#define isUnaryToken(t) ((t) == TK_SUB || (t) == TK_NOT || (t) == TK_INC || (t) == TK_DEC)
#define isOperator(t)   (((t) >= TK_INC && (t) <= TK_NOT) || isUnary(t))

static int precedence(token_type_t operator) {
	switch(operator) {
		case TK_NEGATE:
		case TK_NOT:
		case TK_PRE_INC:
		case TK_PRE_DEC:
			return 4;
		
		case TK_MUL:
		case TK_DIV:
		case TK_MOD:
			return 2;
		
		case TK_ADD:
		case TK_SUB:
			return 1;
		
		case TK_EQ:
		case TK_ASSIGN:
		case TK_ADD_EQ:
		case TK_MIN_EQ:
		case TK_MUL_EQ:
		case TK_DIV_EQ:
			return 0;
		
		default:
			return 42;
	}
}

static int associativity(token_type_t operator) {
	switch (operator) {
		case TK_ASSIGN:
		case TK_ADD_EQ:
		case TK_MIN_EQ:
		case TK_MUL_EQ:
		case TK_DIV_EQ:
			return 0;
		
		default:
			return 1;
	}
}

static ast_node_t *allocNode(winterAllocator_t allocator, size_t numNodes) {
	//TODO: better allocator
	ast_node_t *ret = allocator(NULL, sizeof(ast_node_t) + sizeof(ast_node_t*) * numNodes);
	ret->numNodes = numNodes;
	ret->nodes = (ast_node_t**)(ret + 1);
	return ret;
}

static ast_node_t *createNode(winterAllocator_t allocator, const token_t *token) {
	ast_node_t *ret = NULL;
	
	if (isExpression(token->type)) {
		ret = allocNode(allocator, 0);
		ret->value = token->value;
	} else if (isUnary(token->type)) {
		ret = allocNode(allocator, 1);
	} else if (isOperator(token->type)) {
		ret = allocNode(allocator, 2);
	} 
	
	ret->type = token->type;
	return ret;
}

ast_node_t *_winter_parseExpression(winterAllocator_t allocator, const char *source, char **endPtr) {
	char *string = (char*)source;
	ast_node_t *top, *append = NULL, *parenthesis = NULL;
	token_t token = {0};
	
	enum {
		expression,
		operator
	} expect = expression;
	
	while (token.type != TK_EOF) {
		size_t forward = _winter_nextToken(allocator, string, &string, &token);
		
		switch (expect) {
			case expression: {
				if (isExpression(token.type)) {
					ast_node_t *node;
					
					if (token.type == TK_LPAREN) {
						node = parenthesis = _winter_parseExpression(allocator, string, &string);
						_winter_nextToken(allocator, string, &string, &token);
						
						if (token.type != TK_RPAREN) {
							return NULL;
						}
					} else {
						node = createNode(allocator, &token);
					}
					
					if (append) {
						append->nodes[append->numNodes - 1] = node;
					} else {
						top = node;
					}
					expect = operator;
					
				} else if (isUnaryToken(token.type)) {
					switch (token.type) {
						case TK_SUB: token.type = TK_NEGATE;  break;
						case TK_INC: token.type = TK_PRE_INC; break;
						case TK_DEC: token.type = TK_PRE_DEC; break;
						default: break;
					}
					ast_node_t *node = createNode(allocator, &token);
					if (append) {
						append->nodes[append->numNodes - 1] = node;
						append = node;
					} else {
						top = append = node;
					}
				} else {
					return NULL;
				}
			} break;
			
			case operator: {
				if (isOperator(token.type)) {
					ast_node_t *node = createNode(allocator, &token);
					
					if (top == parenthesis || (associativity(top->type) && precedence(top->type) >= precedence(token.type))) {
						node->nodes[0] = top;
						top = append = node;
					} else {
						ast_node_t *current = top;
						if (associativity(token.type)) {
							while (current->nodes[1] != parenthesis &&
								precedence(current->nodes[1]->type) < precedence(token.type)) {
								
								current = current->nodes[1];
							}
						} else {
							current = append;
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

ast_node_t *_winter_parseStatement(winterAllocator_t allocator, const char *source, char **endPtr) {
	return _winter_parseExpression(allocator, source, endPtr);
}

ast_node_t *generateTreeThing(winterState_t *state, const char *source) {
	char *string;
	return _winter_parseStatement(state->allocator, source, &string);
}

static winterInt_t branchToInt(winterState_t *state, ast_node_t *branch) {
	switch (branch->type) {
		case TK_VALUE: {
			winterObject_t thing;
			_winter_objectToInt(&thing, &branch->value);
			return thing.integer;
		} break;
		
		case TK_IDENT: {
			winterInt_t ret = _winter_tableToInt(&state->globalState, branch->value.string);
			state->allocator(branch->value.string, 0);
			return ret;
		}
		default: return 0;
	}
}

ast_node_t *execute(winterState_t *state, ast_node_t *tree) {
	if (tree && isOperator(tree->type)) {
		ast_node_t *branch1 = execute(state, tree->nodes[0]);
		ast_node_t *branch2 = NULL;
		if (tree->numNodes > 1) {
			branch2 = execute(state, tree->nodes[1]);
		}
		switch (tree->type) {
			case TK_ADD:
				_winter_objectAdd(&tree->value, &branch1->value, &branch2->value);
				break;
			case TK_SUB:
				_winter_objectSub(&tree->value, &branch1->value, &branch2->value);
				break;
			case TK_NEGATE:
				tree->value.integer = -branchToInt(state, branch1);
				break;
			case TK_NOT:
				tree->value.integer = !branchToInt(state, branch1);
				break;
			case TK_PRE_INC:
				if (branch1->type == TK_IDENT) {
					winterInt_t integer = tree->value.integer = _winter_tableToInt(&state->globalState, branch1->value.string) + 1;
					_winter_tableInsertInt(state->allocator, &state->globalState, branch1->value.string, integer);
					state->allocator(branch1->value.string, 0);
				} else {
					tree->value.integer = 0;
				}
				break;
			case TK_MUL:
				tree->value.integer = branchToInt(state, branch1) * branchToInt(state, branch2);
				break;
			case TK_DIV:
				tree->value.integer = branchToInt(state, branch1) / branchToInt(state, branch2);
				break;
			case TK_MOD:
				tree->value.integer = branchToInt(state, branch1) % branchToInt(state, branch2);
				break;
			case TK_EQ:
				tree->value.integer = branchToInt(state, branch1) == branchToInt(state, branch2);
				break;
			case TK_ADD_EQ:
				if (branch1->type == TK_IDENT) {
					winterInt_t integer = _winter_tableToInt(&state->globalState, branch1->value.string) + branchToInt(state, branch2);
					_winter_tableInsertInt(state->allocator, &state->globalState, branch1->value.string, integer);
					tree->value.integer = integer;
					state->allocator(branch1->value.string, 0);
				} else {
					tree->value.integer = 0;
				}
				break;
			case TK_MIN_EQ:
				if (branch1->type == TK_IDENT) {
					winterInt_t integer = _winter_tableToInt(&state->globalState, branch1->value.string) - branchToInt(state, branch2);
					_winter_tableInsertInt(state->allocator, &state->globalState, branch1->value.string, integer);
					tree->value.integer = integer;
					state->allocator(branch1->value.string, 0);
				} else {
					tree->value.integer = 0;
				}
				break;
			case TK_MUL_EQ:
				if (branch1->type == TK_IDENT) {
					winterInt_t integer = _winter_tableToInt(&state->globalState, branch1->value.string) * branchToInt(state, branch2);
					_winter_tableInsertInt(state->allocator, &state->globalState, branch1->value.string, integer);
					tree->value.integer = integer;
					state->allocator(branch1->value.string, 0);
				} else {
					tree->value.integer = 0;
				}
				break;
			case TK_DIV_EQ:
				if (branch1->type == TK_IDENT) {
					winterInt_t integer = _winter_tableToInt(&state->globalState, branch1->value.string) / branchToInt(state, branch2);
					_winter_tableInsertInt(state->allocator, &state->globalState, branch1->value.string, integer);
					tree->value.integer = integer;
					state->allocator(branch1->value.string, 0);
				} else {
					tree->value.integer = 0;
				}
				break;
			case TK_ASSIGN:
				if (branch1->type == TK_IDENT) {
					winterObject_t *object;
					if (branch2->type == TK_IDENT) {
						object = _winter_tableGetObject(&state->globalState, branch2->value.string);
						state->allocator(branch2->value.string, 0);
					} else {
						object = &branch2->value;
					}
					_winter_tableInsert(state->allocator, &state->globalState, branch1->value.string, object);
					tree->value = *object;
					state->allocator(branch1->value.string, 0);
				} else {
					tree->value.integer = 0;
				}
				break;
			default: break;
		}
		state->allocator(branch1, 0);
		state->allocator(branch2, 0);
		tree->type = TK_VALUE;
		tree->numNodes = 0;
		
	}
	return tree;
}
