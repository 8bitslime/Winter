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
#include <stdio.h>

#define lengthOf(a) (sizeof(a) / sizeof(*(a)))
#define isAlphaUnder(c) (isalpha(c) || (c) == '_')
#define isAlNumUnder(c) (isalnum(c) || (c) == '_')
#define isHex(c) (isdigit(c) || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))

static const char *keywords[] = {
	"for", "do", "while", "break", "if", "else", "return"
};

static const char *operators[] = {
	"++", "--", "-=", "+=", "==", "!=", "<=", ">=", "||", "&&", //all multicharacter operators
	"=<>+-*/%|&!" //single character operators in last string
};

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
	for (size_t i = 0; i < lengthOf(operators) - 1; i++) {
		//2 character operator hard coded coded at the moment
		if (strncmp(source, operators[i], 2) == 0) {
			*type = i + TK_INC;
			return 2;
		}
	}
	
	//Check if it is a single character operator
	const char *single_operators = operators[lengthOf(operators) - 1];
	for (int i = 0; i < strlen(single_operators); i++) {
		if (*source == single_operators[i]) {
			*type = i + TK_ASSIGN;
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
		
		//Add 1 to i if source[i] is not a null terminator
		//TODO: throw lexer error if no closing quote found
		return i + !!source[i];
	}
	return 0;
}

//Returns non-zero if the immediate token is a character literal
static size_t isCharacter(const char *source) {
	//TODO: more escape sequence (ie hex)
	if (source[0] == '\'') {
		if (source[1] == '\\' && source[3] == '\'') {
			return 4;
		} else if (source[2] == '\'' && source[1] != '\'') {
			return 3;
		}
		return 0;
	}
}

static size_t nextToken(const char *source, char **endPtr) {
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
	if (endPtr) {
		*endPtr = (char*)(source + ret);
	}
	if (ret) {
		//construct token
		char string[128] = {0};
		memcpy(string, source, ret);
		printf("%s\t%i\t%i\n", string, type, ret);
	}
	return ret;
}

int main(int argc, char **argv) {
	char *string = "for (int i = '\\n'; i < 20; i++);";
	while(nextToken(string, &string));
	return 0;
}
