/* a few aligned allocations */
#include <assert.h>
#include <stdlib.h>
#include "mem.h"

int main() {
	assert(Mem_Init(40960) == 0);
	Mem_Dump();
  
	void* ptr = Mem_Alloc(4040);
	Mem_Dump();
  	
	void* ptr2 = Mem_Alloc(4040);

	//Mem_Dump();
	
	Mem_Free(ptr);
	
	//Mem_Dump();
	
	Mem_Free(ptr2);
	
	//Mem_Dump();
	
	exit(0);
}

