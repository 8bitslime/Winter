#include "winter.h"
#include "wlex.h"
#include "wtable.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
	winterTable_t table;
	_winter_tableAlloc(&table, 100);
	_winter_tableInsertInt(&table, "hello", 12);
	_winter_tableInsertFloat(&table, "world", 100.34);
	
	printf("int at \"hello\": %f\n", _winter_tableToFloat(&table, "hello"));
	printf("int at \"world\": %i\n", (int)_winter_tableToInt(&table, "world"));
	
	return 0;
}
