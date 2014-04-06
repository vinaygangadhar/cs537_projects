/* a few aligned allocations */
#include <assert.h>
#include <stdlib.h>
#include "mem.h"

int main() {
	assert(Mem_Init(40960) == 0);
	Mem_Dump();
	assert(Mem_Alloc(4040) != NULL);
	Mem_Dump();
	assert(Mem_Alloc(16) != NULL);
	assert(Mem_Alloc(32) != NULL);
	assert(Mem_Alloc(8) != NULL);
	exit(0);
}

