#include "lexer.h"
#include "wtype.h"
#include <string.h>
#include <stdlib.h>

#define STRING (lex->string + lex->cursor.pos)
#define LENGTH(a) (sizeof(a) / sizeof(*(a)))

#define isSpace(c)  ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\v' || (c) == '\f' || (c) == '\r')
#define isAlpha(c)  (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
#define isNumber(c) ((c) >= '0' && (c) <= '9')
#define isAlnum(c)  (isAlpha(c) || isNumber(c))
#define isHex(c)    (isNumber(c) || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))
#define isBinary(c) ((c) == '0' || (c) == '1')

static bool_t skipComments(lexState_t *lex) {
	if (STRING[0] == '/') {
		if (STRING[1] == '/') {
			while (*STRING != '\n') {
				lex->cursor.pos++;
				lex->cursor.carrot++;
			}
		} //else if (STRING[1] == '*') {
			
		// }
		return true;
	}
	return false;
}
static void skipWhitespace(lexState_t *lex) {
	do {
		while(*STRING && isSpace(*STRING)) {
			lex->cursor.carrot++;
			if (*STRING == '\n') {
				lex->cursor.line++;
				lex->cursor.carrot = 0;
			}
			lex->cursor.pos++;
		}
	} while (skipComments(lex));
}

//TODO: better lookup data structure
static const char *keywords[] = {
	"for", "do", "while", "if", "else"
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

const char *symbol3[] = { "<<=", ">>=" };
const char *symbol2[] = {
	"++", "--", "**",
	"+=", "-=", "*=", "/=", "%=",
	"||", "&&",
	"|=", "&=", "^=",
	"<<", ">>",
	"!=", "<=", ">=", "=="
};
const char symbols[] = ".,=+-*/%;<>(){}[]&|!^~";
static size_t lexSymbol(lexState_t *lex) {
	for (size_t i = 0; i < LENGTH(symbol3); i++) {
		if (strncmp(STRING, symbol3[i], 3) == 0) {
			lex->lookahead.size = 3;
			lex->lookahead.type = TK_LSHIFTEQ + i;
			return 3;
		}
	}
	
	for (size_t i = 0; i < LENGTH(symbol2); i++) {
		if (strncmp(STRING, symbol2[i], 2) == 0) {
			lex->lookahead.size = 2;
			lex->lookahead.type = TK_INC + i;
			return 2;
		}
	}
	
	for (size_t i = 0; i < LENGTH(symbols); i++) {
		if (symbols[i] == STRING[0]) {
			lex->lookahead.size = 1;
			lex->lookahead.type = STRING[0];
			return 1;
		}
	}
	
	return 0;
}

static winterInt_t strtob(const char *binary) {
	winterInt_t out = 0;
	if (binary[0] == '0' && (binary[1] == 'b' || binary[1] == 'B')) {
		binary += 2;
		while (isBinary(*binary)) {
			out <<= 1;
			out |= (*binary - '0');
			binary++;
		}
	}
	return out;
}

static size_t lexNumber(lexState_t *lex) {
	size_t size = 0;
	if (isNumber(STRING[size])) {
		lex->lookahead.type = TK_INT;
		size = 1;
		
		//Check if it's a hex or binary literal
		if (STRING[0] == '0' && (STRING[1] == 'x' || STRING[1] == 'X') && isHex(STRING[2])) {
			size = 3;
			while (isHex(STRING[size])) size++;
			lex->lookahead.integer = (winterInt_t)strtoll(lex->string + lex->cursor.pos, NULL, 0);
		} else if (STRING[0] == '0' && (STRING[1] == 'b' || STRING[1] == 'B') && isBinary(STRING[2])) {
			size = 3;
			while (STRING[size] == '0' || STRING[size] == '1') size++;
			lex->lookahead.integer = strtob(lex->string + lex->cursor.pos);
		} else {
			while (isNumber(STRING[size])) size++;
			if (STRING[size] == '.') {
				lex->lookahead.type = TK_FLOAT;
				do {
					size++;
				} while (isNumber(STRING[size]));
				lex->lookahead.floating = (winterFloat_t)strtod(lex->string + lex->cursor.pos, NULL);
			} else {
				lex->lookahead.integer = (winterInt_t)strtoll(lex->string + lex->cursor.pos, NULL, 0);
			}
		}
	}
	lex->lookahead.size = size;
	return size;
}

static size_t lexString(lexState_t *lex) {
	size_t size = 0;
	if (STRING[size] == '"') {
		size = 1;
		while(STRING[size] && STRING[size++] != '"') {
			if (STRING[size] == '\\') size++;
		}
		lex->lookahead.size = size;
		lex->lookahead.type = TK_STRING;
	}
	return size;
}

static size_t lexChar(lexState_t *lex) {
	size_t size = 0;
	if (STRING[0] == '\'') {
		lex->lookahead.type = TK_INT;
		if (STRING[1] == '\\') {
			//escape sequence
			if (isNumber(STRING[2])) {
				size = 3;
				while (isNumber(STRING[size])) size++;
				if (STRING[size] != '\'') {
					size = 0;
				} else {
					size++;
					lex->lookahead.integer = (winterInt_t)strtoll(lex->string + lex->cursor.pos + 2, NULL, 8);
				}
			}
			if ((STRING[2] == 'x' || STRING[2] == 'X') && isHex(STRING[3])) {
				size = 4;
				while (isHex(STRING[size])) size++;
				if (STRING[size] != '\'') {
					size = 0;
				} else {
					size++;
					lex->lookahead.integer = (winterInt_t)strtoll(lex->string + lex->cursor.pos + 3, NULL, 16);
				}
			} else if (STRING[3] == '\'') {
				size = 4;
				switch (STRING[2]) {
					case 'a':
						lex->lookahead.integer = '\a';
						break;
					case 'b':
						lex->lookahead.integer = '\b';
						break;
					case 'e':
						lex->lookahead.integer = 0x1B;
						break;
					case 'f':
						lex->lookahead.integer = '\f';
						break;
					case 'n':
						lex->lookahead.integer = '\n';
						break;
					case 'r':
						lex->lookahead.integer = '\r';
						break;
					case 't':
						lex->lookahead.integer = '\t';
						break;
					case 'v':
						lex->lookahead.integer = '\v';
						break;
						
					default:
						lex->lookahead.integer = STRING[2];
						break;
				}
			}
		} else if (STRING[2] == '\'') {
			size = 3;
			lex->lookahead.integer = STRING[2];
		}
	}
	lex->lookahead.size = size;
	return size;
}

static size_t lexIdent(lexState_t *lex) {
	size_t size = 0;
	if (isAlpha(STRING[size]) || STRING[size] == '_') {
		size = 1;
		while (isAlpha(STRING[size]) || isNumber(STRING[size]) || STRING[size] == '_') {
			size++;
		}
		lex->lookahead.size = size;
		lex->lookahead.type = TK_IDENT;
	}
	return size;
}

int _winter_lexNext(lexState_t *lex) {
	skipWhitespace(lex);
	
	lex->current = lex->lookahead;
	lex->lookahead.cursor = lex->cursor;
	
	if (*STRING != '\0') {
		
		if (!(lexKeyword(lex) || lexIdent(lex) || lexNumber(lex) || lexString(lex) || lexChar(lex) || lexSymbol(lex))) {
			lex->lookahead.type = TK_UNKNOWN;
			lex->lookahead.size = 1;
		}
		
		lex->cursor.pos    += lex->lookahead.size;
		lex->cursor.carrot += lex->lookahead.size;
	} else {
		lex->lookahead.type = TK_EOF;
		lex->lookahead.size = 0;
	}
	
	return lex->current.size;
}
