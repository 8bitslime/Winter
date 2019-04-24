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
	
	table_t *table = _winter_tableAlloc(state, 3);
	
	wstring_t string1 = AUTO_STRING("hello world");
	wstring_t string2 = AUTO_STRING("other string");
	object_t value1 = { .type=TYPE_INT, .integer=13 };
	object_t value2 = { .type=TYPE_INT, .integer=140 };
	
	_winter_tableInsert(state, table, &string1, &value1);
	_winter_tableInsert(state, table, &string2, &value2);
	object_t *ptr = _winter_tableGetObject(table, &string2);
	
	if (ptr != NULL) {
		printf("value: %llu\n", ptr->integer);
	}
	_winter_tableFree(state, table);
	
	printf("Winter interpreter v0.1\n");
	
	while (1) {
		printf(">> ");
		if (fgets(buffer, 512, stdin) == NULL) continue;
		if (strncmp(buffer, "exit\n", 5) == 0) break;
		
		ast_node_t *node = _winter_generateTree(state, buffer);
		if (node != NULL) {
			node = walkTree(state, node);
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
