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

	ptr[1] = Mem_Alloc(16);
	assert(ptr[1] != NULL);
	Mem_Dump();

 	assert(Mem_Free(ptr[0]) == 0);
		Mem_Dump();
  

 	assert(Mem_Free(ptr[1]) == 0);
	Mem_Dump();
	
	ptr[2] = Mem_Alloc(32);
	assert(ptr[2] != NULL);
	Mem_Dump();
  
	ptr[3] = Mem_Alloc(8);
	assert(ptr[3] != NULL);
	Mem_Dump();
  
 	assert(Mem_Free(ptr[2]) == 0);
	Mem_Dump();
	
	
 	assert(Mem_Free(ptr[3]) == 0);
	Mem_Dump();
	//printf("ptr: %p\n", ptr);
	
	exit(0);
}

