#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

static int allocs = 0;
static int frees  = 0;

void *allocator(void *ptr, size_t size) {
	void *ret = NULL;
	if (size) {
		if (ptr == NULL) {
			allocs++;
		}
		ret = realloc(ptr, size);
	} else {
		if (ptr != NULL) {
			frees++;
			free(ptr);
		}
	}
	return ret;
}

ast_node_t *walkTree(ast_node_t *node) {
	if (isOperator(node->type)) {
		ast_node_t *left  = walkTree(node->children[0]);
		ast_node_t *right = NULL;
		if (node->numNodes == 2) {
			right = walkTree(node->children[1]);
		}
		switch (node->type) {
			case AST_ADD:
				node->value.integer = left->value.integer + right->value.integer;
				break;
			case AST_SUB:
				node->value.integer = left->value.integer - right->value.integer;
				break;
			case AST_MUL:
				node->value.integer = left->value.integer * right->value.integer;
				break;
			case AST_DIV:
				node->value.integer = left->value.integer / right->value.integer;
				break;
			case AST_POW:
				node->value.integer = (int)(pow(left->value.integer, right->value.integer) + 0.5);
				break;
			case AST_NEGATE:
				node->value.integer = -left->value.integer;
				break;
			case AST_NOT:
				node->value.integer = !left->value.integer;
				break;
			case AST_PASS:
				node->value.integer = left->value.integer;
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
	
	printf("Winter interpreter v0.1\n");
	
	while (1) {
		printf(">> ");
		if (fgets(buffer, 512, stdin) == NULL) continue;
		if (strncmp(buffer, "exit\n", 5) == 0) break;
		
		ast_node_t *node = _winter_generateTree(state, buffer);
		if (node != NULL) {
			node = walkTree(node);
			printf("%llu\n", node->value.integer);
		}
		
		allocator(node, 0);
	}
	
	winterFreeState(state);
	
	printf("allocations:  %i\n", allocs);
	printf("frees:        %i\n", frees);
	printf("difference:   %i\n", allocs - frees);
}
