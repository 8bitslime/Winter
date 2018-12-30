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

#define isExpression(t) ((t) == TK_IDENT || (t) == TK_VALUE || (t) == TK_LPAREN)
#define isUnary(t)      ((t) == TK_NOT || (t) == TK_NEGATE || (t) == TK_PRE_INC || (t) == TK_PRE_DEC)
#define isUnaryToken(t) ((t) == TK_SUB || (t) == TK_NOT || (t) == TK_INC || (t) == TK_DEC)
#define isOperator(t)   (((t) >= TK_INC && (t) <= TK_NOT) || isUnary(t))

struct operator_info {
	int precedence;
	enum {
		LEFT = 0,
		RIGHT = 1
	} associativity;
	void (*function)(void);
};

#define opinfo(p, a, f, t) {(p), (a), (void(*)(void))(f)}
static struct operator_info op_table[] = {
	{0}, // TK_UNKNOWN
	opinfo(9, RIGHT, NULL, TK_IDENT),
	opinfo(9, RIGHT, NULL, TK_VALUE),
	opinfo(6, RIGHT, _winter_objectNegate, TK_NEGATE),
	opinfo(6, RIGHT, _winter_objectPreInc, TK_PRE_INC),
	opinfo(6, RIGHT, _winter_objectPreDec, TK_PRE_DEC),
	opinfo(0, RIGHT, NULL, TK_INC),
	opinfo(0, RIGHT, NULL, TK_DEC),
	opinfo(5, RIGHT, _winter_objectPow, TK_EXP),
	opinfo(0, LEFT,  NULL, TK_MIN_EQ),
	opinfo(0, LEFT,  NULL, TK_ADD_EQ),
	opinfo(0, LEFT,  NULL, TK_MUL_EQ),
	opinfo(0, LEFT,  NULL, TK_DIV_EQ),
	opinfo(1, RIGHT, _winter_objectEqual, TK_EQ),
	opinfo(1, RIGHT, NULL, TK_NEQ),
	opinfo(2, RIGHT, NULL, TK_LEQ),
	opinfo(2, RIGHT, NULL, TK_GEQ),
	opinfo(1, RIGHT, NULL, TK_OR),
	opinfo(0, RIGHT, NULL, TK_AND),
	opinfo(6, LEFT,  NULL, TK_DOT),
	opinfo(0, LEFT,  _winter_objectAssign, TK_ASSIGN),
	opinfo(2, RIGHT, NULL, TK_LESS),
	opinfo(2, RIGHT, NULL, TK_GREAT),
	opinfo(3, RIGHT, _winter_objectAdd, TK_ADD),
	opinfo(3, RIGHT, _winter_objectSub, TK_SUB),
	opinfo(4, RIGHT, _winter_objectMul, TK_MUL),
	opinfo(4, RIGHT, _winter_objectDiv, TK_DIV),
	opinfo(4, RIGHT, _winter_objectMod, TK_MOD),
	opinfo(1, RIGHT, NULL, TK_BITOR),
	opinfo(1, RIGHT, NULL, TK_BITAND),
	opinfo(6, RIGHT, _winter_objectNot, TK_NOT),
};

#define precedence(t) (op_table[t].precedence)
#define associativity(t) (op_table[t].associativity)

static ast_node_t *allocNode(winterState_t *state, int numNodes) {
	//TODO: better allocator
	ast_node_t *ret = MALLOC(sizeof(ast_node_t) + sizeof(ast_node_t*) * numNodes);
	ret->numNodes = numNodes;
	ret->nodes = (ast_node_t**)(ret + 1);
	return ret;
}

static ast_node_t *resizeNode(winterState_t *state, ast_node_t *node, int newSize) {
	//TODO: failed allocation checking
	ast_node_t *ret = REALLOC(node, sizeof(ast_node_t) + sizeof(ast_node_t*) * newSize);
	ret->numNodes = newSize;
	ret->nodes = (ast_node_t**)(ret + 1);
	return ret;
}

static ast_node_t *createNode(winterState_t *state, const token_t *token) {
	ast_node_t *ret = NULL;
	
	if (isExpression(token->type)) {
		ret = allocNode(state, 0);
		ret->value = token->value;
	} else if (isUnary(token->type)) {
		ret = allocNode(state, 1);
	} else if (isOperator(token->type)) {
		ret = allocNode(state, 2);
	}
	
	ret->type = token->type;
	return ret;
}

ast_node_t *_winter_parseExpression(winterState_t *state, lexState_t *lex) {
	ast_node_t *top = NULL, *append = NULL, *parenthesis = NULL;
	token_t token = {0};
	
	enum {
		expression,
		operator
	} expect = expression;
	
	while (token.type != TK_EOF) {
		_winter_nextToken(state, lex);
		token = lex->current;
		
		switch (expect) {
			case expression: {
				if (isExpression(token.type)) {
					ast_node_t *node;
					
					if (token.type == TK_LPAREN) {
						node = parenthesis = _winter_parseExpression(state, lex);
						if (lex->current.type != TK_RPAREN) {
							return NULL;
						}
					} else {
						node = createNode(state, &token);
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
					ast_node_t *node = createNode(state, &token);
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
					ast_node_t *node = createNode(state, &token);
					
					if (top == parenthesis || (associativity(top->type) && precedence(top->type) >= precedence(token.type))) {
						node->nodes[0] = top;
						top = append = node;
					} else {
						ast_node_t *current = top;
						if (associativity(token.type) == RIGHT) {
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
					return top;
				}
			} break;
		}
	}
	return top;
}

static ast_node_t *declareVar(winterState_t *state, lexState_t *lex) {
	if (lex->lookahead.type == TK_VAR) {
		_winter_nextToken(state, lex);
		ast_node_t *ret = allocNode(state, 0);
		ret->type = TK_VAR;
		while (lex->lookahead.type == TK_IDENT) {
			_winter_nextToken(state, lex);
			
			ret = resizeNode(state, ret, ret->numNodes + 1);
			ast_node_t *ident = createNode(state, &lex->current);
			ret->nodes[ret->numNodes - 1] = ident;
			
			switch (lex->lookahead.type) {
				case TK_SEMICOLON:
					_winter_nextToken(state, lex);
					return ret;
				case TK_COMMA:
					_winter_nextToken(state, lex);
					continue;
				case TK_ASSIGN: {
					ast_node_t *assign = createNode(state, &lex->lookahead);
					_winter_nextToken(state, lex);
					assign->nodes[0] = ident;
					assign->nodes[1] = _winter_parseExpression(state, lex);
					ret->nodes[ret->numNodes - 1] = assign;
					// _winter_nextToken(state, lex);
					continue;
				default: return NULL;
				}
			}
		}
		return ret;
	}
	return NULL;
}

ast_node_t *_winter_parseStatement(winterState_t *state, lexState_t *lex) {
	ast_node_t *ret = NULL;
	
	if ((ret = declareVar(state, lex))) {
		// _winter_nextToken(state, lex);
	} else {
		ret = _winter_parseExpression(state, lex);
	}
	
	if (lex->current.type == TK_SEMICOLON) {
		// _winter_nextToken(state, lex);
		return ret;
	} // else expected ';'!
	
	//free tree
	return NULL;
}

ast_node_t *generateTreeThing(winterState_t *state, const char *source) {
	lexState_t lex = {source};
	_winter_nextToken(state, &lex);
	
	ast_node_t *ret = allocNode(state, 0);
	ret->type = TK_STATEMENT;
	while (lex.current.type != TK_EOF) {
		ast_node_t *statement = _winter_parseStatement(state, &lex);
		if (statement != NULL) {
			ret = resizeNode(state, ret, ret->numNodes + 1);
			ret->nodes[ret->numNodes-1] = statement;
		} else {
			//check for parse error then free resources
		}
	}
	return ret;
}

typedef int (*binary_op)(winterObject_t *, winterObject_t *, winterObject_t *);
typedef int (*unary_op)(winterObject_t *, winterObject_t *);

ast_node_t *execute(winterState_t *state, ast_node_t *tree) {
	if (tree && tree->type == TK_STATEMENT) {
		ast_node_t *node = NULL;
		for (int i = 0; i < tree->numNodes; i++) {
			FREE(node);
			if (tree->nodes[i] == NULL) {
				printf("error: null node!\n");
			}
			node = execute(state, tree->nodes[i]);
		}
		FREE(tree);
		//TODO: push node to stack
		return node;
	} else if (tree && tree->type == TK_VAR) {
		for (int i = 0; i < tree->numNodes; i++) {
			if (tree->nodes[i]->type == TK_IDENT) {
				_winter_tableInsert(state, &state->globalState, tree->nodes[i]->value.string, NULL);
				FREE(tree->nodes[i]->value.string);
			} else if (tree->nodes[i]->type == TK_ASSIGN) {
				ast_node_t *left = tree->nodes[i]->nodes[0];
				ast_node_t *right = execute(state, tree->nodes[i]->nodes[1]);
				_winter_tableInsert(state, &state->globalState, left->value.string, &right->value);
				FREE(left->value.string);
				FREE(left);
				FREE(right);
			}
			FREE(tree->nodes[i]);
		}
		tree->type = TK_VALUE;
		tree->value = nullObject;
		tree->numNodes = 0;
	} else if (tree && isOperator(tree->type)) {
		ast_node_t *branches[2]; //hard coded for the moment
		winterObject_t *vals[2];
		for (int i = 0; i < tree->numNodes; i++) {
			branches[i] = execute(state, tree->nodes[i]);
			vals[i] = &branches[i]->value;
			if (branches[i]->type == TK_REF) {
				vals[i] = vals[i]->pointer;
			}
		}
		
		if (isUnary(tree->type)) {
			((unary_op) op_table[tree->type].function)(&tree->value, vals[0]);
		} else {
			((binary_op)op_table[tree->type].function)(&tree->value, vals[0], vals[1]);
		}
		
		for (int i = 0; i < tree->numNodes; i++) {
			FREE(branches[i]);
		}
		tree->type = TK_VALUE;
		tree->numNodes = 0;
	} else if (tree->type == TK_IDENT) {
		tree->type = TK_REF;
		tree->value.type = TYPE_REF;
		winterObject_t *ref = _winter_tableGetObject(&state->globalState, tree->value.string);
		FREE(tree->value.string);
		tree->value.pointer = ref;
	}
	return tree;
}
