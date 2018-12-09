#include "winter.h"
#include "wlex.h"
#include "wtable.h"
#include "wparse.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
	char buffer[512];
	
	while (strncmp(buffer, "exit", 4)) {
		fgets(buffer, 512, stdin);
		
		ast_node_t *ast = generateTreeThing(buffer);
		ast = execute(ast);
		
		printf("%i\n", (int)ast->value.integer);
		free(ast);
	}
	
	return 0;
}
