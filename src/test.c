#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

void * allocator(void *ptr, size_t size) {
	void *ret = NULL;
	if (size) {
		ret = realloc(ptr, size);
	} else {
		free(ptr);
	}
	return ret;
}

int main(int argc, char **argv) {
	
	winterState_t *state = winterCreateState(allocator);
	ast_node_t *node = _winter_generateTree(state, argv[1]);
	
	printf("thing: %i\n", node->value);
	
	// while (_winter_lexNext(&lex)) {
	// 	switch (lex.current.type) {
	// 		case TK_IDENT:
	// 			printf("ident: '%.*s'", (int)lex.current.size, lex.string + lex.current.cursor.pos);
	// 			break;
	// 		case TK_STRING:
	// 			printf("string: %.*s", (int)lex.current.size, lex.string + lex.current.cursor.pos);
	// 			break;
	// 		case TK_FOR:
	// 		case TK_DO:
	// 		case TK_WHILE:
	// 		case TK_IF:
	// 		case TK_ELSE:
	// 			printf("keyword: %.*s", (int)lex.current.size, lex.string + lex.current.cursor.pos);
	// 			break;
	// 		case TK_INT:
	// 			printf("int: %llu", lex.current.integer);
	// 			break;
	// 		case TK_FLOAT:
	// 			printf("float: %f", lex.current.floating);
	// 			break;
	// 		case TK_UNKNOWN:
	// 			printf("unknown");
	// 			break;
	// 		default:
	// 			printf("symbol: %c", lex.current.type);
	// 			break;
	// 	}
	// 	printf ("\tat x: %llu, y: %llu\n", lex.current.cursor.carrot, lex.current.cursor.line);
	// }
}
