/*
** FILE: wlex.c
**
** PURPOSE: Contains all functions necessary for splitting a string into tokens
**          and categorizing them.
**
** Copyright (c) 2018 Zachary Wells, see LICENSE for more details
*/

#include "wlex.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define lengthOf(a) (sizeof(a) / sizeof(*(a)))
#define isAlphaUnder(c) (isalpha(c) || (c) == '_')
#define isAlNumUnder(c) (isalnum(c) || (c) == '_')
#define isHex(c) (isdigit(c) || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))

static const char *keywords[] = {
	"for", "do", "while", "break", "if", "else", "return"
};

static const char *operators[] = {
	//all multicharacter operators
	"++", "--", "-=", "+=", "==", "!=", "<=", ">=", "||", "&&",
};	

static const char* single_operators = ".=<>+-*/%|&!";

static const char *symbols = ",;()[]{}";

//Advances the source pointer forward past whitespaces
static void skipWhitespaces(const char **source) {
	while(isspace(**source)) (*source)++;
}

//Returns non-zero if the immediate token is a comment
static size_t skipComments(const char *source) {
	if (source[0] == '/') {
		size_t i = 2;
		switch (source[1]) {
			case '/':
				while (source[i] && source[i] != '\n') i++;
				return i;
			
			case '*':
				while (source[i] && strncmp(source + i, "*/", 2) != 0) i++;
				return source[i] ? i + 2 : i;
		}
	}
	return 0;
}

//Returns non-zero if the immediate token is a keyword
static size_t isKeyword(const char *source, token_type_t *type) {
	for (size_t i = 0; i < lengthOf(keywords); i++) {
		size_t length = strlen(keywords[i]);
		if (strncmp(source, keywords[i], length) == 0) {
			if (isAlNumUnder(source[length])) {
				continue;
			}
			*type = TK_FOR + i;
			return length;
		}
	}
	return 0;
}

//Returns non-zero if the immediate token is a symbol
static size_t isSymbol(const char *source, token_type_t *type) {
	for (int i = 0; i < strlen(symbols); i++) {
		if (*source == symbols[i]) {
			*type = i + TK_COMMA;
			return 1;
		}
	}
	return 0;
}

//Returns non-zero if the immediate token is an operator
//Multicharacter operators take priority in tokenization
static size_t isOperator(const char *source, token_type_t *type) {
	//Check multicharcter operator
	for (size_t i = 0; i < lengthOf(operators); i++) {
		//2 character operator hard coded coded at the moment
		if (strncmp(source, operators[i], 2) == 0) {
			*type = i + TK_INC;
			return 2;
		}
	}
	
	//Check if it is a single character operator
	for (int i = 0; i < strlen(single_operators); i++) {
		if (*source == single_operators[i]) {
			*type = i + TK_DOT;
			return 1;
		}
	}
	return 0;
}

//Returns non-zero if the immediate token is an identifier
static size_t isIdentifier(const char *source) {
	if (isAlphaUnder(source[0])) {
		size_t i = 1;
		while(isAlNumUnder(source[i])) i++;
		return i;
	}
	return 0;
}
s os off
//Returns non-zero if the immedaite token is a number
static size_t isNumber(const char *source, token_type_t *type) {
	size_t i = 0;
	if (isdigit(source[0])) {
		*type = TK_INT;
		if (source[0] == '0' && (source[1] == 'x' || source[1] == 'X') && isdigit(source[2])) {
			i+=3;
			while(isHex(source[i])) i++;
		} else {
			while(isdigit(source[i])) i++;
			if (source[i] == '.') {
				i++;
				*type = TK_FLOAT;
				while(isdigit(source[i])) i++;
			}
		}
	}
	return i;
}

//Returns non-zero if the immedaite token is a string
static size_t isString(const char *source) {
	if (*source == '"') {
		size_t i = 1;
		
		while(source[i] && source[i] != '"') {
			if (source[i] == '\\') {
				i += 2;
			} else {
				i++;
			}
		}
		
		if (source[i] == '"') {
			//Only return non-zero if closing quotation mark
			return i + 1;
		}
	}
	return 0;
}

//Returns non-zero if the immediate token is a character literal
static size_t isCharacter(const char *source) {
	if (source[0] == '\'') {
		if (source[1] == '\\') {
			if (source[2] == 'x') {
				//hex escape
				size_t i = 3;
				while (isHex(source[i])) i++;
				if (source[i] == '\'') {
					return i + 1;
				}
			} else if (isdigit(source[2])) {
				size_t i = 3;
				while (isdigit(source[i])) i++;
				if (source[i] == '\'') {
					return i + 1;
				}
			} else if (source[3] == '\'') {
				return 4;
			}
		} else if (source[2] == '\'' && source[1] != '\'') {
			return 3;
		}
	}
	return 0;
}

//Returns the length of the escape sequence and sets character to the ascii value
static size_t decodeEscape(const char *source, winter_int *character) {
	if (source[0] == '\\') {
		switch (source[1]) {
			case 'a':
				*character = '\a';
				return 2;
			case 'b':
				*character = '\b';
				return 2;
			case 'f':
				*character = '\f';
				return 2;
			case 'n':
				*character = '\n';
				return 2;
			case 'r':
				*character = '\r';
				return 2;
			case 't':
				*character = '\t';
				return 2;
			case 'v':
				*character = '\v';
				return 2;
			case 'e':
				*character = '\e';
				return 2;
			
			case 'x': { //hex escape sequence
				char *pointer;
				*character = (char)strtoll(source+2, &pointer, 16);
				return pointer - source;
			}
		}
		if (isdigit(source[1])) { //octal escape sequence
			char *pointer;
			*character = (char)strtoll(source+1, &pointer, 8);
			return source - pointer + 1;
		} else {
			*character = source[1];
			return 2;
		}
	}
	return 0;
}

static size_t nextToken(const char *source, char **endPtr, token_t *token) {
	size_t ret = 0;
	token_type_t type;
	
	int comment;
	do {
		skipWhitespaces(&source);
		comment = skipComments(source);
		source += comment;
	} while(comment);
	
	if (*source) {
		if (isAlphaUnder(*source)) {
			if ((ret = isKeyword(source, &type))) goto end;
			if ((ret = isIdentifier(source))) {
				type = TK_IDENT;
				goto end;
			}
		} else {
			if ((ret = isNumber(source, &type))) goto end;
			if ((ret = isSymbol(source, &type))) goto end;
			if ((ret = isOperator(source, &type))) goto end;
			if ((ret = isString(source))) {
				type = TK_STRING;
				goto end;
			}
			if ((ret = isCharacter(source))) {
				type = TK_CHAR;
				goto end;
			}
		}
		
		type = TK_UNKNOWN;
		ret = 1;
	}
	
	end:
	if (ret) {
		if (endPtr) {
			*endPtr = (char*)(source + ret);
		}
		
		token->type = type;
		switch (type) {
			case TK_INT: {
				token->info.integer = (winter_int)strtoll(source, NULL, 0);
			} break;
			
			case TK_FLOAT: {
				token->info.floating = (winter_float)strtod(source, NULL);
			} break;
			
			case TK_IDENT: {
				token->info.string = malloc(ret + 1);
				memcpy(token->info.string, source, ret);
				token->info.string[ret] = '\0';
			} break;
			
			case TK_STRING: {
				//TODO: allow custom allocators
				token->info.string = malloc(ret);
				source++;
				
				size_t i = 0;
				while (*source != '"') {
					if (*source == '\\') {
						winter_int character;
						source += decodeEscape(source, &character);
						token->info.string[i++] = character;
					} else {
						token->info.string[i++] = *source;
						source++;
					}
				}
				
				token->info.string[i+1] = '\0';
			} break;
			
			case TK_CHAR: {
				if (source[1] == '\\') {
					decodeEscape(source + 1, &token->info.integer);
				} else {
					token->info.integer = (winter_int)source[1];
				}
			} break;
			
			default: break;
		}
	}
	
	return ret;
}

#if 1
//useful for debug printing
static const char *token_names[] = {
	"TK_UNKNOWN",
	"TK_FOR", "TK_DO", "TK_WHILE", "TK_BREAK",
	"TK_IF", "TK_ELSE", "TK_RETURN",
	"TK_COMMA", "TK_SEMICOLON",
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
#endif

int main(int argc, char **argv) {
	token_t token;
	char *string = "x = \"hello \\a world\"";
	
	while(nextToken(string, &string, &token)) {
		switch (token.type) {
			case TK_IDENT:
			case TK_STRING:
				printf("%s: %s\n", token_names[token.type], token.info.string);
				free(token.info.string);
				break;
			case TK_INT:
				printf("TK_INT: %i\n", (int)token.info.integer);
				break;
			case TK_FLOAT:
				printf("TK_FLOAT: %f\n", token.info.floating);
				break;
			case TK_CHAR:
				printf("TK_CHAR: %i\n", (int)token.info.integer);
				break;
			default:
				printf("%s\n", token_names[token.type]);
				break;
		}
	}
	
	return 0;
}
