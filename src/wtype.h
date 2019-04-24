#ifndef WTYPE_H
#define WTYPE_H

#include "winter.h"

typedef struct winterState_t {
	winterAlloc_t allocator;
} winterState_t;

#define MALLOC(s)     (state->allocator(NULL, (s)))
#define REALLOC(p, s) (state->allocator((p),  (s)))
#define FREE(p)       (state->allocator((p),    0))

typedef int bool_t;
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

#define isSpace(c)  ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\v' || (c) == '\f' || (c) == '\r')
#define isAlpha(c)  (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
#define isNumber(c) ((c) >= '0' && (c) <= '9')
#define isAlnum(c)  (isAlpha(c) || isNumber(c))
#define isHex(c)    (isNumber(c) || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))
#define isOctal(c)  ((c) >= '0' && (c) <= '7')
#define isBinary(c) ((c) == '0' || (c) == '1')

//Similar to strtoi, but less error checking. For strings known to be formatted only!
static inline winterInt_t winter_strtoi(const char *number, int base) {
	winterInt_t out = 0;
	switch(base) {
		case 2:
			while (isNumber(*number)) {
				out <<= 1;
				out |= *number - '0';
				number++;
			}
			break;
		case 8:
		case 10:
			while (isNumber(*number)) {
				out *= base;
				out += *number - '0';
				number++;
			}
			break;
		case 16:
			while (isHex(*number)) {
				out *= base;
				if (*number >= 'a')
					out += *number - 'a' + 10;
				else if (*number >= 'A')
					out += *number - 'A' + 10;
				else
					out += *number - '0';
				number++;
			}
			break;
	}
	return out;
}
static inline winterFloat_t winter_strtof(const char *number) {
	winterFloat_t out = 0;
	
	while (isNumber(*number)) {
		out *= 10;
		out += *number - '0';
		number++;
	}
	if (*number == '.') {
		number++;
		winterFloat_t divisor = 1;
		while (isNumber(*number)) {
			divisor *= 0.1;
			out += (winterFloat_t)(*number - '0') * divisor;
			number++;
		}
	}
	
	return out;
}

#endif
