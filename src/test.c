#include "lexer.h"
#include "parser.h"
#include "wstring.h"
#include "table.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

ast_node_t *walkTree(winterState_t *state, ast_node_t *tree);

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
			node = walkTree(state, node);
			if (node->type == AST_REFERENCE) {
				node->value = *(object_t*)(node->value.pointer);
			}
			switch (node->value.type) {
				case TYPE_INT:
					printf("%lli\n", node->value.integer);
					break;
				case TYPE_FLOAT:
					printf("%f\n", node->value.floating);
					break;
				default:
					printf("null\n");
					break;
			}
		}
		
		allocator(node, 0);
	}
	
	winterFreeState(state);
	
	printf("allocations:  %i\n", allocs);
	printf("frees:        %i\n", frees);
	printf("difference:   %i\n", allocs - frees);
}
