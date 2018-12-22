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
	
	while (strcmp(buffer, "exit\n")) {
		
		if (buffer[0]) {
			ast_node_t *ast = generateTreeThing(state, buffer);
			ast = execute(state, ast);
			
			if (ast == NULL) {
				printf("ERROR\n");
			} else {
				winterObject_t *object = &ast->value;
				if (ast->type == TK_IDENT) {
					object = _winter_tableGetObject(&state->globalState, ast->value.string);
				}
				if (object == NULL) {
					printf("ERROR\n");
				} else {
					switch (object->type) {
						case TYPE_INT:
							printf("%i\n", (int)object->integer);
							break;
						case TYPE_FLOAT:
							printf("%f\n", object->floating);
							break;
						case TYPE_STRING:
							printf("\"%s\"\n", object->string);
						default: break;
					}
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
