#include "parser.h"
#include "wtype.h"

#include <stdio.h>

//creates or resizes a node
static ast_node_t *allocNode(winterState_t *state, ast_node_t *node, size_t size) {
	ast_node_t *ret = state->allocator(NULL, sizeof(ast_node_t) + sizeof(ast_node_t*) * size);
	ret->numNodes = size;
	ret->children = (ast_node_t**)(ret + 1);
	return ret;
}

static ast_node_t *createEprNode(winterState_t *state, const token_t *token) {
	ast_node_t *ret = allocNode(state, NULL, 0);
	ret->type = AST_VALUE;
	
	//TODO: convert value to object
	ret->value = token->integer;
	return ret;
}

static ast_node_t *createOprNode(winterState_t *state, ast_node_type_t type) {
	size_t size = 2;
	if (isUnary(type)) {
		size = 1;
	}
	ast_node_t *ret = allocNode(state, NULL, size);
	ret->type = type;
	return ret;
}

static int precedence(ast_node_type_t operator) {
	switch (operator) {
		case AST_ADD:
		case AST_SUB:
			return 1;
		case AST_MUL:
		case AST_DIV:
		case AST_MOD:
			return 2;
		case AST_POW:
			return 3;
		
		default: return 999;
	}
} 

static inline ast_node_t *parseExpression(winterState_t *state, lexState_t *lex) {
	ast_node_t *tree = NULL;
	ast_node_t *tail = NULL;
	
	enum {
		expression,
		operator
	} expect = expression;
	
	while (_winter_lexNext(lex)) {
		token_t *token = &lex->current;
		
		if (expect == expression) {
			//expression
			if (isExpression(token->type)) {
				//Add expression to bottom of tree
				ast_node_t *node = createEprNode(state, token);
				if (tree == NULL) {
					tree = node;
				} else {
					tail->children[tail->numNodes - 1] = node;
				}
				expect = operator;
			} else if (isUnarySymbol(token->type)) {
				//Add unary operator
				//TODO: unary precedence
				if (token->type == '-') {
					token->type = AST_NEGATE;
				}
				ast_node_t *node = createOprNode(state, token->type);
				if (tree == NULL) {
					tree = node;
				} else {
					tail->children[tail->numNodes - 1] = node;
				}
				tail = node;
				//Don't change expect because we still want an expression next
				
			} else {
				//TODO: error handling and unary operators
				printf("expected expression\n");
				return NULL;
				//Eventually going to make an error type node to return
			}
			
		} else {
			//operator
			if (isOperator(token->type)) {
				ast_node_t *node = createOprNode(state, token->type);
				int priority = precedence(node->type);
				
				//operator precedence in place
				if (priority <= precedence(tree->type)) {
					node->children[0] = tree;
					tree = node;
					tail = node;
				} else {
					//higher precedence goes down the tree
					ast_node_t *walk = tree;
					while (priority > precedence(walk->children[1]->type)) {
						walk = walk->children[1];
					}
					node->children[0] = walk->children[1];
					walk->children[1] = node;
					tail = node;
				}
				
				expect = expression;
			} else {
				//current expression is done
				break;
			}
		}
	}
	
	return tree;
}

ast_node_t *_winter_generateTree(winterState_t *state, const char *source) {
	lexState_t lex = {source};
	_winter_lexNext(&lex);
	return parseExpression(state, &lex);
}
