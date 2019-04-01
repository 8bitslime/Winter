#ifndef WTYPE_H
#define WTYPE_H

#include "winter.h"

typedef struct winterState_t {
	winterAlloc_t allocator;
} winterState_t;

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

#endif
