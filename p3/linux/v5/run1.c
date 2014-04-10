/* a few aligned allocations */
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "mem.h"

int main() {
	assert(Mem_Init(4096) == 0);
	Mem_Dump();
 
	void* ptr[4];
	
	ptr[0] = Mem_Alloc(8);
	assert(ptr[0] != NULL);
	Mem_Dump();

 	assert(Mem_Free(ptr[0]) == 0);
	Mem_Dump();
	
	exit(0);
}

