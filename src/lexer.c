#include "lexer.h"
#include <string.h>

#define STRING (lex->string)
#define LENGTH(a) (sizeof(a) / sizeof(*(a)))

#define isSpace(c)  ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\v' || (c) == '\f' || (c) == '\r')
#define isAlpha(c)  (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
#define isNumber(c) ((c) >= '0' && (c) <= '9')
#define isAlnum(c)  (isAlpha(c) || isNumber(c))

static size_t skipWhitespace(lexState_t *lex) {
	size_t i = 0;
	while(STRING[i] && isSpace(STRING[i])) {
		lex->cursor.carrot++;
		if (STRING[i] == '\n') {
			lex->cursor.line++;
			lex->cursor.carrot = 0;
		}
		i++;
	}
	lex->cursor.pos += i;
	return i;
}

//TODO: better lookup data structure
static const char *keywords[] = {
	"for"
};

static size_t lexKeyword(lexState_t *lex) {
	lex->lookahead.size = 0;
	for (size_t i = 0; i < LENGTH(keywords); i++) {
		size_t length = strlen(keywords[i]);
		if (!strncmp(STRING, keywords[i], length) && !isAlnum(STRING[length])) {
			lex->lookahead.type = TK_FOR + i;
			lex->lookahead.size = length;
		}
	}
	return lex->lookahead.size;
}

static size_t lexIdent(lexState_t *lex) {
	size_t start = lex->cursor.pos;
	size_t size = 0;
	if (isAlpha(STRING[size])) {
		size = 1;
		while (isAlpha(STRING[start + size]) || isNumber(STRING[start + size])) {
			size++;
		}
		lex->lookahead.size = size;
		lex->lookahead.type = TK_IDENT;
	}
	return size;
}

int _winter_lexNext(lexState_t *lex) {
	lex->current = lex->lookahead;
	lex->lookahead.cursor = lex->cursor;
	
	if (lex->string[lex->cursor.pos] != '\0') {
		if (lexIdent(lex));
		
		lex->cursor.pos    += lex->lookahead.size;
		lex->cursor.carrot += lex->lookahead.size;
	} else {
		lex->lookahead.type = TK_EOF;
		lex->lookahead.size = 0;
	}
	
	return lex->current.size;
}
