#include "parser.h"
#include "wstring.h"
#include "table.h"
#include <stdio.h>

//creates or resizes a node
//TODO: maybe make custom allocator
//TODO: node resizing
static inline ast_node_t *allocNode(winterState_t *state, ast_node_t *node, size_t size) {
	size_t original = 0;
	if (node != NULL) {
		original = node->numNodes;
	}
	
	ast_node_t *ret = REALLOC(node, sizeof(ast_node_t) + sizeof(ast_node_t*) * size);
	ret->numNodes = size;
	ret->children = (ast_node_t**)(ret + 1);
	for (size_t i = original; i < size; i++) {
		ret->children[i] = NULL;
	}
	return ret;
}

static inline ast_node_t *createEprNode(winterState_t *state, const token_t *token) {
	ast_node_t *ret;
	if (token->type == TK_LPAREN) {
		ret = allocNode(state, NULL, 1);
		ret->type = AST_PASS;
	} else {
		ret = allocNode(state, NULL, 0);
		switch (token->type) {
			case TK_IDENT: ret->type = AST_IDENT; break;
			default:       ret->type = AST_VALUE; break;
		}
		_winter_tokenToObject(state, token, &ret->value);
	}
	return ret;
}

static inline ast_node_t *createOprNode(winterState_t *state, ast_node_type_t type) {
	size_t size = isUnary(type) ? 1 : 2;
	ast_node_t *ret = allocNode(state, NULL, size);
	ret->type = type;
	return ret;
}

static inline ast_node_t *createErrorNode(winterState_t *state) {
	ast_node_t *ret = allocNode(state, NULL, 0);
	ret->type = AST_ERROR;
	return ret;
}

static inline void freeTree(winterState_t *state, ast_node_t *tree) {
	if (tree != NULL) {
		for (size_t i = 0; i < tree->numNodes; i++) {
			freeTree(state, tree->children[i]);
		}
		if (isManaged(tree->type)) { 
			_winter_objectDelRef(state, &tree->value);
		}
		FREE(tree);
	}
}

typedef void (*func_ptr_void_t)(void);
typedef struct opinfo_t {
	int precedence;
	enum { left, right } associativity;
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
	op(AST_POW,      5, left,  _winter_objectPow),
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
	op(AST_ASSIGN,   1, left,  _winter_objectAssign),
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
	
	//Will either return a proper expression or error
	if (isExpression(lex->lookahead.type)) {
		while (lex->lookahead.type != TK_EOF) {
			token_t *token = &lex->lookahead;
			
			if (expect == expression) {
				//expression
				if (isExpression(token->type)) {
					//Add expression to bottom of tree
					ast_node_t *node = createEprNode(state, token);
					
					if (token->type == TK_LPAREN) {
						//parenthesis parsing
						_winter_lexNext(lex);
						ast_node_t *parens = parseExpression(state, lex);
						
						//TODO: make this more elegant
						if (parens == NULL) {
							ast_node_t *error = createErrorNode(state);
							_winter_objectNewError(state, &error->value, "expected an expression");
							freeTree(state, tree);
							FREE(node);
							return error;
						} else if (parens->type == AST_ERROR) {
							freeTree(state, tree);
							FREE(node);
							return parens;
						} else if (lex->lookahead.type != TK_RPAREN) {
							ast_node_t *error = createErrorNode(state);
							_winter_objectNewError(state, &error->value, "expected closing parenthesis");
							freeTree(state, tree);
							freeTree(state, parens);
							FREE(node);
							return error;
						}
						
						node->children[0] = parens;
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
					//Don't change 'expect' because we still want an expression next
					
				} else {
					if (tree == NULL) {
						return NULL;
					} else {
						ast_node_t *error = createErrorNode(state);
						//TODO: line numbers and other debug stuff
						_winter_objectNewError(state, &error->value, "expected an expression");
						freeTree(state, tree);
						return error;
					}
				}
				
			} else {
				//operator
				if (isOperator(token->type)) {
					ast_node_t *node = createOprNode(state, token->type);
					int priority = precedence(node->type);
					
					//operator precedence in place
					if (isExpression(tree->type) || (associativity(node->type) && priority <= precedence(tree->type))) {
						node->children[0] = tree;
						tree = node;
						tail = node;
					} else {
						ast_node_t *append = tree;
						//check associativity
						if (associativity(node->type) == left) {
							append = tail;
						} else {
							//higher precedence goes down the tree
							while (priority > precedence(append->children[append->numNodes - 1]->type)) {
								append = append->children[append->numNodes - 1];
							}
						}
						node->children[0] = append->children[append->numNodes - 1];
						append->children[append->numNodes - 1] = node;
						tail = node;
					}
					
					expect = expression;
				} else {
					//current expression is done
					break;
				}
			}
			_winter_lexNext(lex);
		}
	}
	return tree;
}

static inline ast_node_t *parseLet(winterState_t *state, lexState_t *lex) {
	size_t size = 0;
	ast_node_t *ret = NULL;
	if (lex->lookahead.type == TK_LET) {
		_winter_lexNext(lex);
			
		while (lex->lookahead.type == TK_IDENT) {
			_winter_lexNext(lex);
			//TODO: check out of memory
			ret = allocNode(state, ret, ++size);
			ret->type = AST_LET;
			ret->children[size - 1] = createEprNode(state, &lex->current);
			
			if (lex->lookahead.type == TK_COMMA) {
				_winter_lexNext(lex);
				continue;
			} else if (lex->lookahead.type == TK_ASSIGN) {
				_winter_lexNext(lex);
				ast_node_t *eq = createOprNode(state, AST_ASSIGN);
				eq->children[0] = ret->children[size - 1];
				ret->children[size - 1] = eq;
				
				ast_node_t *expr = parseExpression(state, lex);
				if (expr == NULL) {
					freeTree(state, ret);
					ret = createErrorNode(state);
					_winter_objectNewError(state, &ret->value, "expected an expression");
				} else if (expr->type == AST_ERROR) {
					freeTree(state, ret);
					ret = expr;
				} else {
					eq->children[1] = expr;
					printf("next token thing: %i\n", lex->lookahead.type);
					if (lex->lookahead.type == TK_COMMA) {
						_winter_lexNext(lex);
						continue;
					}
				}
			}
			
			return ret;
		}
		
		//Incorrect let statement
		freeTree(state, ret);
		ret = createErrorNode(state);
		_winter_objectNewError(state, &ret->value, "expected an identifier");
	}
	return ret;
}

static inline ast_node_t *parseStatement(winterState_t *state, lexState_t *lex) {
	ast_node_t *statement = NULL;
	
	//TODO: probably just use goto here, this is weird
	do {
		statement = parseLet(state, lex);
		if (statement != NULL) break;
		
		statement = parseExpression(state, lex);
		if (statement != NULL) break;
	} while (0);
	
	if (statement != NULL && statement->type == AST_ERROR) {
		return statement;
	}
	
	if (lex->lookahead.type == TK_SEMICOLON) {
		_winter_lexNext(lex);
	} else {
		freeTree(state, statement);
		statement = createErrorNode(state);
		_winter_objectNewError(state, &statement->value, "expected a semicolon");
	}
	
	return statement;
}

static inline ast_node_t *parseBlock(winterState_t *state, lexState_t *lex, token_type_t stop) {
	size_t size = 0;
	ast_node_t *block = NULL;
	while (lex->lookahead.type != stop) {
		ast_node_t *temp = parseStatement(state, lex);
		if (temp != NULL) {
			if (temp->type == AST_ERROR) {
				freeTree(state, block);
				return temp;
			}
			
			block = allocNode(state, block, ++size);
			block->type = AST_BLOCK;
			block->children[size - 1] = temp;
		}
	}
	_winter_lexNext(lex);
	return block;
}

ast_node_t *_winter_generateTree(winterState_t *state, const char *source) {
	lexState_t lex = {source};
	_winter_lexNext(&lex);
	return parseBlock(state, &lex, TK_EOF);
}

//Temporary for testing
ast_node_t *walkTree(winterState_t *state, ast_node_t *node) {
	if (node->type == AST_BLOCK) {
		ast_node_t *result = NULL;
		for (size_t i = 0; i < node->numNodes; i++) {
			freeTree(state, result);
			result = walkTree(state, node->children[i]);
			node->children[i] = NULL;
			if (result->type == AST_ERROR) break;
		}
		freeTree(state, node);
		return result;
	} else if (isOperator(node->type)) {
		ast_node_t *nodes[2] = {
			walkTree(state, node->children[0]),
			NULL
		};
		
		//check if the left side is an error
		if (nodes[0]->type == AST_ERROR) {
			node->children[0] = NULL;
			freeTree(state, node);
			return nodes[0];
		}
		
		if (node->numNodes == 2) {
			nodes[1] = walkTree(state, node->children[1]);
			
			//check if the right side is an error
			if (nodes[1]->type == AST_ERROR) {
				node->children[1] = NULL;
				freeTree(state, node);
				return nodes[1];
			}
		}
		
		typedef int (*binary)(winterState_t *state, object_t *, object_t *);
		typedef int (*unary) (winterState_t *state, object_t *);
		
		int result = OBJECT_OK;
		
		//TODO: remove PASS during optimization phase
		if (nodes[0]->type != AST_PASS) {
			//DEBUG ONLY
			if (function(node->type) == NULL) {
				printf("function not implemented! %i\n", node->type);
				result = OBJECT_ERROR_TYPE;
			} else if (isUnary(node->type)) {
				result = ((unary)function(node->type))(state, &nodes[0]->value);
			} else {
				result = ((binary)function(node->type))(state, &nodes[0]->value, &nodes[1]->value);
			}
		}
		
		if (result != OBJECT_OK) {
			_winter_objectDelRef(state, &nodes[0]->value);
			//TODO: better formatting, example: "operator '+' incompatible with types 'table' and 'int'"
			//Possible line numbers and other debugging stuff
			_winter_objectNewError(state, &node->value, "incompatible type");
			node->type = AST_ERROR;
		} else {
			node->value = nodes[0]->value;
			node->type = AST_VALUE;
		}
		node->numNodes = 0;
		
		FREE(nodes[0]);
		if (nodes[1] != NULL) {
			_winter_objectDelRef(state, &nodes[1]->value);
			FREE(nodes[1]);
		}
	} else if (node->type == AST_PASS) {
		ast_node_t *out = walkTree(state, node->children[0]);
		FREE(node);
		node = out;
	} else if (node->type == AST_LET) {
		for (size_t i = 0; i < node->numNodes; i++) {
			ast_node_t *key_node = node->children[i];
			object_t *key = NULL;
			object_t *value = NULL;
			if (key_node->type == AST_ASSIGN) {
				key = &key_node->children[0]->value;
				key_node->children[1] = walkTree(state, key_node->children[1]);
				
				if (key_node->children[1]->type == AST_ERROR) {
					ast_node_t *temp = key_node->children[1];
					key_node->children[1] = NULL;
					freeTree(state, node);
					return temp;
				}
				
				value = &(key_node->children[1]->value);
			} else {
				key = &node->value;
			}
			object_t *object = _winter_tableGetObject(state->globals, key);
			if (object != NULL) {
				_winter_objectNewError(state, &node->value, "mutiple declarations of '%s'", key->string->data);
				freeTree(state, node->children[i]);
				node->type = AST_ERROR;
				node->numNodes = 0;
				return node;
			} else {
				_winter_tableInsert(state, state->globals, key, value);
				freeTree(state, node->children[i]);
			}
		}
		node->type  = AST_VALUE; 
		node->numNodes = 0;
		node->value = (object_t){TYPE_NULL};
	} else if (node->type == AST_IDENT) {
		object_t *obj = _winter_tableGetObject(state->globals, &node->value);
		if (obj == NULL) {
			//Undeclared identifier
			object_t error;
			_winter_objectNewError(state, &error, "undeclared identifier '%s'", node->value.string->data);
			_winter_objectDelRef(state, &node->value);
			node->type = AST_ERROR;
			node->value = error;
		} else {
			_winter_objectDelRef(state, &node->value);
			node->type = AST_VALUE;
			node->value.type = TYPE_REFERENCE;
			node->value.pointer = obj;
		}
	}
	return node;
}
