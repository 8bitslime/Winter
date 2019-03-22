#include "parser.h"
#include "wtype.h"

static ast_node_t *allocNode(winterState_t *state, size_t size) {
	ast_node_t *ret = state->allocator(NULL, sizeof(ast_node_t) + sizeof(ast_node_t*) * size);
	ret->numNodes = size;
	ret->children = (ast_node_t**)(ret + 1);
	return ret;
}

static ast_node_t *createEprNode(winterState_t *state, const token_t *token) {
	ast_node_t *ret = allocNode(state, 0);
	ret->type = token->type;
	ret->value = token->integer;
	return ret;
}

static inline ast_node_t *parseExpression(winterState_t *state, lexState_t *lex) {
	ast_node_t *tree = NULL;
	
	enum {
		expression,
		operator
	} expect = expression;
	
	while (_winter_lexNext(lex)) {
		token_t *token = &lex->current;
		
		if (expect == expression) {
			if (isExpression(token->type)) {
				ast_node_t *value = createEprNode(state, token);
				tree = value;
				break;
			} else {
				//TODO: error handling
				if (tree) {
					return NULL;
				} else {
					return NULL;
				}
			}
		} else {
			
		}
	}
	
	return tree;
}

ast_node_t *_winter_generateTree(winterState_t *state, const char *source) {
	lexState_t lex = {source};
	_winter_lexNext(&lex);
	return parseExpression(state, &lex);
}
