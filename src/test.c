#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void * allocator(void *ptr, size_t size) {
	void *ret = NULL;
	if (size) {
		ret = realloc(ptr, size);
	} else {
		free(ptr);
	}
	return ret;
}

ast_node_t *walkTree(ast_node_t *node) {
	if (isOperator(node->type)) {
		ast_node_t *left  = walkTree(node->children[0]);
		ast_node_t *right = walkTree(node->children[1]);
		switch (node->type) {
			case AST_ADD:
				node->value = left->value + right->value;
				break;
			case AST_SUB:
				node->value = left->value - right->value;
				break;
			case AST_MUL:
				node->value = left->value * right->value;
				break;
			case AST_DIV:
				node->value = left->value / right->value;
				break;
			case AST_POW:
				node->value = left->value;
				for (int i = right->value; i > 1; i--) {
					node->value *= left->value;
				}
				break;
			default: break;
		}
		allocator(left, 0);
		allocator(right, 0);
		
		node->type = AST_VALUE;
	}
	return node;
}

int main(int argc, char **argv) {
	winterState_t *state = winterCreateState(allocator);
	char buffer[512] = {0};
	
	printf("Winter interpreter v0.1b\n");
	
	while (1) {
		printf(">> ");
		if (fgets(buffer, 512, stdin) == NULL) continue;
		if (strncmp(buffer, "exit\n", 5) == 0) break;
		
		ast_node_t *node = _winter_generateTree(state, buffer);
		if (node != NULL) {
			node = walkTree(node);
		}
		
		printf("%i\n", node->value);
		allocator(node, 0);
	}
	
	winterFreeState(state);
}
