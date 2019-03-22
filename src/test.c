#include "lexer.h"
#include <stdio.h>
#include <inttypes.h>

int main(int argc, char **argv) {
	lexState_t lex = {
		"'\\xFF'"
	}; _winter_lexNext(&lex);
	
	while (_winter_lexNext(&lex)) {
		switch (lex.current.type) {
			case TK_IDENT:
				printf("ident: '%.*s'", (int)lex.current.size, lex.string + lex.current.cursor.pos);
				break;
			case TK_STRING:
				printf("string: %.*s", (int)lex.current.size, lex.string + lex.current.cursor.pos);
				break;
			case TK_FOR:
			case TK_DO:
			case TK_WHILE:
			case TK_IF:
			case TK_ELSE:
				printf("keyword: %.*s", (int)lex.current.size, lex.string + lex.current.cursor.pos);
				break;
			case TK_INT:
				printf("int: %llu", lex.current.integer);
				break;
			case TK_FLOAT:
				printf("float: %f", lex.current.floating);
				break;
			case TK_UNKNOWN:
				printf("unknown");
				break;
			default:
				printf("symbol: %c", lex.current.type);
				break;
		}
		printf ("\tat x: %llu, y: %llu\n", lex.current.cursor.carrot, lex.current.cursor.line);
	}
}
