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

static inline void printObject(object_t *obj) {
	switch (obj->type) {
		case TYPE_UNKNOWN: printf("unknown object!"); break;
		case TYPE_ERROR:   printf("error: %s", obj->string->data); break;
		case TYPE_NULL:    printf("null"); break;
		case TYPE_REFERENCE: printf("ref -> "); printObject(obj->pointer); break;
		case TYPE_INT:     printf("int: %lli", obj->integer); break;
		case TYPE_FLOAT:   printf("float: %f", obj->floating); break;
		case TYPE_STRING:  printf("string: \"%s\"", obj->string->data); break;
		default: printf("object type: %i", obj->type); break;
	}
}

static inline void printAST(ast_node_t *tree, int level) {
	for (int i = 1; i <= level; i++) {
		printf("    |");
	}
	if (isOperator(tree->type) || tree->type == AST_PASS) {
		printf("opr: %i\n", tree->type);
		for (int i = 0; i < tree->numNodes; i++) {
			printAST(tree->children[i], level + 1);
		}
	} else if (tree->type == AST_BLOCK) {
		printf("block:\n");
		for (size_t i = 0; i < tree->numNodes; i++) {
			printAST(tree->children[i], level + 1);
		}
	} else if (tree->type == AST_IDENT) {
		printf("variable: \"%s\"\n", tree->value.string->data);
	} else if (tree->type == AST_VALUE) {
		printObject(&tree->value);
		printf("\n");
	} else if (tree->type == AST_LET) {
		printf("let:\n");
		for (size_t i = 0; i < tree->numNodes; i++) {
			printAST(tree->children[i], level + 1);
		}
	} else {
		printf("unkown node type: %i\n", tree->type);
	}
}

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
		
		size_t length = strlen(buffer);
		if (buffer[length-2] != ';') {
			buffer[length-1]   = ';';
		} else {
			buffer[length-1] = '\0';
		}
		
		ast_node_t *node = _winter_generateTree(state, buffer);
		if (node != NULL) {
			printAST(node, 0);
			
			node = walkTree(state, node);
			
			printObject(&node->value);
			printf("\n");
			_winter_objectDelRef(state, &node->value);
			FREE(node);
		}
	}
	
	winterFreeState(state);
	
	printf("allocations:  %i\n", allocs);
	printf("frees:        %i\n", frees);
	printf("difference:   %i\n", allocs - frees);
}
