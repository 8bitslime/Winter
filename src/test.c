#include "winter.h"
#include "wstate.h"
#include "wlex.h"
#include "wtable.h"
#include "wparse.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int allocations, frees;

void *allocator(void *pointer, size_t newSize) {
	if (newSize) {
		// printf("Allocated %I64i bytes\n", newSize);
		if (pointer == NULL) {
			allocations++;
		}
		return realloc(pointer, newSize);
	} else {
		// printf("Freed some memory\n");
		if (pointer != NULL) {
			frees++;
			free(pointer);
		}
		return NULL;
	}
}

int main(int argc, char **argv) {
	printf("Winter Interpreter v0.0.1\n");
	
	char buffer[512] = {0};
	winterState_t *state = winterCreateState(allocator);
	
	while (strcmp(buffer, "exit;\n")) {
		
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
						case TYPE_NULL:
							printf("null\n");
							break;
						case TYPE_INT:
							printf("%I64i\n", object->integer);
							break;
						case TYPE_FLOAT:
							printf("%f\n", object->floating);
							break;
						case TYPE_STRING:
							printf("\"%s\"\n", object->string);
						default: break;
					}
				}
				FREE(ast);
			}
		}
		
		printf(">> ");
		fgets(buffer, 512, stdin);
		size_t len = strlen(buffer);
		buffer[len-1] = ';';
		buffer[len]   = '\n';
		buffer[len+1] = '\0';
	}
	winterFreeState(state);
	
	printf("alloctions: %i\nfrees: %i\n", allocations, frees);
	return 0;
}
