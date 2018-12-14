#include "winter.h"
#include "wstate.h"
#include "wlex.h"
#include "wtable.h"
#include "wparse.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void *allocator(void *pointer, size_t newSize) {
	if (newSize) {
		// printf("Allocated %I64i bytes\n", newSize);
		return realloc(pointer, newSize);
	} else {
		// printf("Freed some memory\n");
		free(pointer);
		return NULL;
	}
}

int main(int argc, char **argv) {
	printf("Winter Interpreter v0.0.1\n");
	
	char buffer[512] = {0};
	winterState_t *state = winterCreateState(allocator);
	
	while (strncmp(buffer, "exit", 4)) {
		
		if (buffer[0]) {
			ast_node_t *ast = generateTreeThing(state, buffer);
			ast = execute(state, ast);
			
			if (ast == NULL) {
				printf("ERROR\n");
			} else {
				switch (ast->type) {
					case TK_INT:
						printf("%i\n", (int)ast->value.integer);
						break;
					case TK_IDENT:
						printf("%s: %i\n", ast->value.string, _winter_tableToInt(&state->globalState, ast->value.string));
				}
				allocator(ast, 0);
			}
		}
		
		printf(">> ");
		fgets(buffer, 512, stdin);
	}
	
	winterFreeState(state);
	return 0;
}
