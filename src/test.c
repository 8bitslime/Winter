#include "winter.h"
#include "wlex.h"
#include "wtable.h"
#include "wparse.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
	char buffer[512] = {0};
	
	while (strncmp(buffer, "exit", 4)) {
		ast_node_t *ast = generateTreeThing(buffer);
		ast = execute(ast);
		
		if (ast != NULL) {
			printf("%i\n", (int)ast->value.integer);
			free(ast);
		}
		
		printf(">> ");
		fgets(buffer, 512, stdin);
	}
	
	return 0;
}
