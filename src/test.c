#include "winter.h"
#include "wlex.h"
#include "wtable.h"
#include "wparse.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
	char buffer[512] = {0};
	winterState_t *state;
	
	while (strncmp(buffer, "exit", 4)) {
		
		if (buffer[0]) {
			ast_node_t *ast = generateTreeThing(state, buffer);
			ast = execute(ast);
			
			if (ast != NULL) {
				printf("%i\n", (int)ast->value.integer);
				free(ast);
			} else {
				printf("ERROR\n");
			}
		}
		
		printf(">> ");
		fgets(buffer, 512, stdin);
	}
	
	return 0;
}
