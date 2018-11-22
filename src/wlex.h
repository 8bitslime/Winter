#ifndef WINLEX_H
#define WINLEX_H

typedef enum token_type_t {
	TOKEN_UNKNOWN = 0,
	TOKEN_FOR, TOKEN_DO, TOKEN_WHILE, TOKEN_BREAK,
	TOKEN_IF, TOKEN_ELSE, TOKEN_RETURN,
	TOKEN_IDENT,
	TOKEN_INT, TOKEN_FLOAT
} token_type_t;

typedef union token_info_t {
	int integer;
	
} token_info_t;

#endif
