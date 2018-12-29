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

#define lengthOf(a) (sizeof(a) / sizeof(*(a)))
#define isAlphaUnder(c) (isalpha(c) || (c) == '_')
#define isAlNumUnder(c) (isalnum(c) || (c) == '_')
#define isHex(c) (isdigit(c) || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))

#define cursor (lex->source + lex->cur)

static const char *keywords[] = {
	"for", "do", "while", "break", "if", "else", "return", "var"
};

static const char *operators[] = {
	//all multicharacter operators
	"++", "--", "**", "-=", "+=", "*=", "/=", "==", "!=", "<=", ">=", "||", "&&",
};	
static const char *single_operators = ".=<>+-*/%|&!";
static const char *symbols = ",;:()[]{}";

//Advances the source pointer forward past whitespaces
static void skipWhitespaces(lexState_t *lex) {
	while(isspace(*cursor)) {
		if (*cursor == '\n') {
			lex->line++;
		}
		lex->cur++;
	}
}

//Returns non-zero if the immediate token is a comment
static bool_t skipComments(lexState_t *lex) {
	if (*cursor == '/') {
		switch (*(cursor + 1)) {
			case '/':
				lex->cur++;
				while (*cursor && *cursor != '\n') lex->cur++;
				lex->line++;
				return true;
			
			case '*':
				lex->cur++;
				while (*cursor && !(cursor[0] == '*' && cursor[1] == '/')) {
					if (*cursor == '\n') {
						lex->line++;
					}
					lex->cur++;
				}
				if (*cursor) {
					lex->cur += 2;
				}
				return true;
		}
	}
	return false;
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
static size_t decodeEscape(const char *source, winterInt_t *character) {
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
				
			//Non-standard escape sequence, but usefull in Linux
			case 'e':
				*character = 0x1B;
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
			return pointer - source;
		} else {
			*character = source[1];
			return 2;
		}
	}
	return 0;
}

size_t _winter_nextToken(winterState_t *state, lexState_t *lex) {
	size_t size = 0;
	token_type_t type;
	lex->current = lex->lookahead;
	
	do {
		skipWhitespaces(lex);
	} while(skipComments(lex));
	
	if (*cursor) {
		if (isAlphaUnder(*cursor)) {
			if ((size = isKeyword(cursor, &type))) goto end;
			if ((size = isIdentifier(cursor))) {
				type = TK_IDENT;
				goto end;
			}
		} else {
			if ((size = isNumber(cursor, &type))) goto end;
			if ((size = isSymbol(cursor, &type))) goto end;
			if ((size = isOperator(cursor, &type))) goto end;
			if ((size = isString(cursor))) {
				type = TK_STRING;
				goto end;
			}
			if ((size = isCharacter(lex->source + lex->cur))) {
				type = TK_CHAR;
				goto end;
			}
		}
		
		type = TK_UNKNOWN;
		size = 1;
	} else {
		type = TK_EOF;
		size = 0;
	}
	
	end:
	lex->lookahead.type = type;
	switch (type) {
		case TK_INT: {
			lex->lookahead.type = TK_VALUE;
			lex->lookahead.value.type = TYPE_INT;
			lex->lookahead.value.integer = (winterInt_t)strtoll(cursor, NULL, 0);
		} break;
		
		case TK_FLOAT: {
			lex->lookahead.type = TK_VALUE;
			lex->lookahead.value.type = TYPE_FLOAT;
			lex->lookahead.value.floating = (winterFloat_t)strtod(cursor, NULL);
		} break;
		
		case TK_IDENT: {
			lex->lookahead.value.string = MALLOC(size + 1);
			memcpy(lex->lookahead.value.string, cursor, size);
			lex->lookahead.value.string[size] = '\0';
		} break;
		
		case TK_STRING: {
			//TODO: create string object
			char *string = (char*)cursor;
			lex->lookahead.value.string = MALLOC(size - 1);
			string++;
			
			size_t i = 0;
			while (*string != '"') {
				if (*string == '\\') {
					winterInt_t character;
					string += decodeEscape(string, &character);
					lex->lookahead.value.string[i++] = character;
				} else {
					lex->lookahead.value.string[i++] = *string;
					string++;
				}
			}
			
			lex->lookahead.type = TK_VALUE;
			lex->lookahead.value.type = TYPE_STRING;
			lex->lookahead.value.string[i] = '\0';
		} break;
		
		case TK_CHAR: {
			lex->lookahead.type = TK_VALUE;
			lex->lookahead.value.type = TYPE_INT;
			if (cursor[1] == '\\') {
				decodeEscape(cursor + 1, &lex->lookahead.value.integer);
			} else {
				lex->lookahead.value.integer = (winterInt_t)cursor[1];
			}
		} break;
		
		default: break;
	}
	lex->cur += size;
	return size;
}
