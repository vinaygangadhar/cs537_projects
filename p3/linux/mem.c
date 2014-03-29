#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "mem.h"

#define DEBUG

#define FILEPATH "/dev/zero"

volatile int mem_call = 0;
int m_error;

//Mem_init
int Mem_Init(int sizeOfRegion)
{
	mem_call++;
	if(mem_call > 1 || sizeOfRegion <= 0)
	{
		m_error = E_BAD_ARGS;
		return (-1);
	}

	//sizeOfRegion (in bytes) needs to be evenly divisible by the page size
	int round_sz = 0;
	int pg_sz = getpagesize();
	int pg_div = (int) sizeOfRegion / pg_sz;

	if(!pg_div)
		round_sz = pg_sz;
	else 
		round_sz = (int) (sizeOfRegion % getpagesize());

	#ifdef DEBUG
		printf("page size: %d\n", pg_sz);
	#endif
	
	//Open a file for mapping
	int fd = open(FILEPATH, O_RDWR);
	
	//Call the mmap func: mmap(addr, len, prot, flags, fildes, off);
	void* mem_ptr = mmap(NULL, sizeOfRegion, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	
	if (mem_ptr == MAP_FAILED)
	{
		m_error = E_BAD_POINTER; 
		return (-1);
	}
	
	//close the device (don't worry, mapping should be unaffected)
	close(fd);
	return 0;

}

void* Mem_Alloc(int size, int style)
{
}


int Mem_Free(void* ptr)
{
}


void Mem_Dump()
{
}
