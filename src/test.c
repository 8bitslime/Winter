#include "winter.h"
#include "wlex.h"
#include "wparser.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static const char *token_names[] = {
	"TK_UNKNOWN",
	"TK_FOR", "TK_DO", "TK_WHILE", "TK_BREAK",
	"TK_IF", "TK_ELSE", "TK_RETURN",
	"TK_COMMA", "TK_SEMICOLON", "TK_COLON",
	"TK_LPAREN", "TK_RPAREN",
	"TK_LBRACKET", "TK_RBRACKET",
	"TK_LCURLY", "TK_RCURLY",
	"TK_INC", "TK_DEC",
	"TK_MIN_EQ", "TK_ADD_EQ", "TK_EQ", "TK_NEQ",
	"TK_LEQ", "TK_GEQ",
	"TK_OR", "TK_AND",
	"TK_DOT", "TK_ASSIGN", "TK_LESS", "TK_GREAT",
	"TK_ADD", "TK_SUB", "TK_MUL", "TK_DIV", "TK_MOD",
	"TK_BITOR", "TK_BITAND", "TK_NOT",
	"TK_IDENT",
	"TK_INT", "TK_FLOAT", "TK_STRING", "TK_CHAR"
};

int main(int argc, char **argv) {
	char buffer[512] = "";
	
	printf("type and expression: ");
	while (strncmp(buffer, "exit", 4) != 0) {
		// char *string = buffer;
		// token_t token;
		// while(_winter_nextToken(string, &string, &token)) {
		// 	switch (token.type) {
		// 		case TK_IDENT:
		// 		case TK_STRING:
		// 			printf("%s: %s\n", token_names[token.type], token.info.string);
		// 			free(token.info.string);
		// 			break;
		// 		case TK_INT:
		// 			printf("TK_INT: %i\n", (int)token.info.integer);
		// 			break;
		// 		case TK_FLOAT:
		// 			printf("TK_FLOAT: %f\n", token.info.floating);
		// 			break;
		// 		case TK_CHAR:
		// 			printf("TK_CHAR: %i\n", (int)token.info.integer);
		// 			break;
		// 		default:
		// 			printf("%s\n", token_names[token.type]);
		// 			break;
		// 	}
		// }
		fgets(buffer, 512, stdin);
		
		ast_node_t *tree = _winter_parseExpression(buffer, NULL);
		tree = _winter_executeAST(tree);
		printf("%i\n", (int)tree->info.integer);
		free(tree);
	}
	
	return 0;
}
