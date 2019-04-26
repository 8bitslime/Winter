
#include "winter.h"
#include "wtype.h"
#include "table.h"

winterState_t *winterCreateState(winterAlloc_t allocator) {
	winterState_t *ret;
	ret = allocator(NULL, sizeof(winterState_t));
	if (ret != NULL) {
		ret->allocator = allocator;
		ret->globals   = _winter_tableAlloc(ret, 3);
	}
	return ret;
}
void winterFreeState(winterState_t *state) {
	_winter_tableFree(state, state->globals);
	FREE(state);
}
