#ifndef WTYPE_H
#define WTYPE_H

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

#endif
