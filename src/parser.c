#include "parser.h"
#include <stdio.h>

//creates or resizes a node
//TODO: maybe make custom allocator
static ast_node_t *allocNode(winterState_t *state, ast_node_t *node, size_t size) {
	ast_node_t *ret = state->allocator(NULL, sizeof(ast_node_t) + sizeof(ast_node_t*) * size);
	ret->numNodes = size;
	ret->children = (ast_node_t**)(ret + 1);
	return ret;
}

static ast_node_t *createEprNode(winterState_t *state, const token_t *token) {
	ast_node_t *ret;
	if (token->type == TK_LPAREN) {
		ret = allocNode(state, NULL, 1);
		ret->type = AST_PASS;
	} else {
		ret = allocNode(state, NULL, 0);
		ret->type = AST_VALUE;
		_winter_tokenToObject(state, token, &ret->value);
	}
	return ret;
}

static ast_node_t *createOprNode(winterState_t *state, ast_node_type_t type) {
	size_t size = isUnary(type) ? 1 : 2;
	ast_node_t *ret = allocNode(state, NULL, size);
	ret->type = type;
	return ret;
}

typedef void (*func_ptr_void_t)(void);
typedef struct opinfo_t {
	int precedence;
	enum { right, left } associativity;
	func_ptr_void_t function;
} opinfo_t;

//I love tables
#define op(t, p, a, f) {p, a, (func_ptr_void_t)f}
static const opinfo_t opinfo[] = {
	//TODO: implement all the NULLs
	op(AST_LSHIFTEQ, 2, right, NULL),
	op(AST_RSHIFTEQ, 2, right, NULL),
	op(TK_INC,       2, right, NULL),
	op(TK_DEC,       2, right, NULL),
	op(AST_POW,      5, left,  NULL),
	op(AST_ADDEQ,    2, left,  NULL),
	op(AST_SUBEQ,    2, left,  NULL),
	op(AST_MULEQ,    2, left,  NULL),
	op(AST_DIVEQ,    2, left,  NULL),
	op(AST_MODEQ,    2, left,  NULL),
	op(AST_OR,       2, right, NULL),
	op(AST_AND,      2, right, NULL),
	op(AST_OREQ,     2, left,  NULL),
	op(AST_ANDEQ,    2, left,  NULL),
	op(AST_XOREQ,    2, left,  NULL),
	op(AST_LSHIFT,   2, right, NULL),
	op(AST_RSHIFT,   2, right, NULL),
	op(AST_NOTEQ,    2, right, NULL),
	op(AST_LEQ,      2, right, NULL),
	op(AST_GEQ,      2, right, NULL),
	op(AST_EQ,       2, right, NULL),
	op(AST_DOT,      2, right, NULL),
	op(AST_COMMA,    2, right, NULL),
	op(AST_ASSIGN,   2, left,  NULL),
	op(AST_ADD,      2, right, _winter_objectAdd),
	op(AST_SUB,      2, right, _winter_objectSub),
	op(AST_MUL,      3, right, _winter_objectMul),
	op(AST_DIV,      3, right, _winter_objectDiv),
	op(AST_MOD,      3, right, _winter_objectMod),
	op(AST_LESS,     2, right, NULL),
	op(AST_GREATER,  2, right, NULL),
	op(AST_BITAND,   2, right, NULL),
	op(AST_BITOR,    2, right, NULL),
	op(AST_NOT,      2, right, NULL),
	op(AST_XOR,      2, right, NULL),
	op(AST_BITNOT,   2, right, NULL),
	op(AST_NEGATE,   6, right, _winter_objectNegate),
};

static inline int precedence(ast_node_type_t operator) {
	//if the type is not an operator then return infinite precedence
	if (operator < AST_LSHIFTEQ) {
		return 999;
	} else {
		return opinfo[operator - AST_LSHIFTEQ].precedence;
	}
}
static inline int associativity(ast_node_type_t operator) {
	return opinfo[operator - AST_LSHIFTEQ].associativity;
}

static inline func_ptr_void_t function(ast_node_type_t operator) {
	return opinfo[operator - AST_LSHIFTEQ].function;
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
				
				if (token->type == TK_LPAREN) {
					//parenthesis parsing
					node->children[0] = parseExpression(state, lex);
					if (lex->current.type != TK_RPAREN) {
						printf("expected closing parenthesis!\n");
						return NULL;
					}
				}
				
				if (tree == NULL) {
					tree = node;
				} else {
					tail->children[tail->numNodes - 1] = node;
				}
				expect = operator;
			} else if (isUnarySymbol(token->type)) {
				//Add unary operator
				//TODO: unary precedence
				if (token->type == TK_SUB) {
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
				printf("expected expression!\n");
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

//Temporary for testing
ast_node_t *walkTree(winterState_t *state, ast_node_t *node) {
	if (isOperator(node->type)) {
		ast_node_t *left  = walkTree(state, node->children[0]);
		ast_node_t *right = NULL;
		if (node->numNodes == 2) {
			right = walkTree(state, node->children[1]);
		}
		
		typedef int (*binary)(object_t *, object_t *);
		typedef int (*unary)(object_t *);
		
		if (node->type >= AST_LSHIFTEQ) {
			if (isUnary(node->type)) {
				((unary)function(node->type))(&left->value);
			} else {
				((binary)function(node->type))(&left->value, &right->value);
			}
		}
		node->value = left->value;
		
		state->allocator(left, 0);
		if (right) {
			state->allocator(right, 0);
		}
		
		node->type = AST_VALUE;
	}
	return node;
}
