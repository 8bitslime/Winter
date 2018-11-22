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
	"++", "--", "-=", "+=", "||", "&&", "=+-*/%|&"
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
			if (isAlphaUnder(source[length])) {
				continue;
			}
			*type = TOKEN_FOR + i;
			return length;
		}
	}
	return 0;
}

//Returns non-zero if the immediate token is a symbol
static size_t isSymbol(const char *source) {
	for (int i = 0; i < strlen(symbols); i++) {
		if (*source == symbols[i]) {
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
	} else {
		return 0;
	}
}

//Returns non-zero if the immedaite token is a number
static size_t isNumber(const char *source, token_type_t *type) {
	size_t i = 0;
	if (isdigit(source[0])) {
		*type = TOKEN_INT;
		if (source[0] == '0' && (source[1] == 'x' || source[1] == 'X') && isdigit(source[2])) {
			i+=3;
			while(isHex(source[i])) i++;
		} else {
			while(isdigit(source[i])) i++;
			if (source[i] == '.') {
				i++;
				*type = TOKEN_FLOAT;
				while(isdigit(source[i])) i++;
			}
		}
	}
	return i;
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
				type = TOKEN_IDENT;
				goto end;
			}
		} else {
			if ((ret = isNumber(source, &type))) goto end;
			if ((ret = isSymbol(source))) goto end;
		}
		
		type = TOKEN_UNKNOWN;
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
		printf("%s\t%i\n", string, type);
	}
	return ret;
}

int main(int argc, char **argv) {
	char *string = "_123 = 123; x = hell0*0x34";
	while(nextToken(string, &string));
	return 0;
}
