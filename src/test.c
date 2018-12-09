#include "winter.h"
#include "wlex.h"
#include "wtable.h"
#include "wparse.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
	ast_node_t *ast = _winter_parseStatement("1 + 2 * 3");
	ast = execute(ast);
	
	printf("1 + 2 * 3 = %i\n", (int)ast->value.integer);
	
	return 0;
}
