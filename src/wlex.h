#ifndef WINLEX_H
#define WINLEX_H

typedef enum token_type_t {
	TK_UNKNOWN = 0,
	TK_FOR, TK_DO, TK_WHILE, TK_BREAK,
	TK_IF, TK_ELSE, TK_RETURN,
	TK_COMMA, TK_SEMICOLON,
	TK_LPAREN, TK_RPAREN,
	TK_LBRACKET, TK_RBRACKET,
	TK_LCURLY, TK_RCURLY,
	TK_IDENT,
	TK_INT, TK_FLOAT, TK_STRING
} token_type_t;

typedef union token_info_t {
	int integer;
	
} token_info_t;

#endif
