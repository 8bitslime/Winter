#include "winter.h"
#include "wtype.h"

winterState_t *winterCreateState(winterAlloc_t allocator) {
	winterState_t *ret;
	ret = allocator(NULL, sizeof(winterState_t));
	if (ret != NULL) {
		ret->allocator = allocator;
	}
	return ret;
}
void winterFreeState(winterState_t *state) {
	state->allocator(state, 0);
}
