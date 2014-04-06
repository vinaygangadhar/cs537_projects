#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#include "mem.h"
#include "helper.h"
#define DEBUG

#define FILEPATH "/dev/zero"


volatile int mem_call = 0;
int m_error = 0;

//Mem_init function initially called
int Mem_Init(int sizeOfRegion)
{
	mem_call++;
	if(mem_call > 1 || sizeOfRegion <= 0)
	{
		m_error = E_BAD_ARGS;
		return (-1);
	}

	//sizeOfRegion (in bytes) needs to be evenly divisible by the page size
	int page_size = getpagesize();
	
	int round_size = (int)(sizeOfRegion / page_size);
	int reminder = sizeOfRegion % page_size;
	if (reminder != 0)
		round_size++;
	
	round_size = round_size * page_size;	

#ifdef DEBUG
	printf("\n");
	printf("Allocated Init size: %d\n", round_size);
#endif
	
	//Open a file for mapping
	int fd = open(FILEPATH, O_RDWR);
	
	//Call the mmap func: mmap(addr, len, prot, flags, fildes, off);
	void* mem_ptr = mmap(NULL, round_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	
	if (mem_ptr == MAP_FAILED)
	{
		m_error = E_BAD_ARGS; 
		return (-1);
	}
	
	//Close the device (don't worry, mapping should be unaffected)
	close(fd);
	
	//Free list initialization	
	head = (node_t*) mem_ptr;
	head->size = round_size;
	head->next = NULL;
	m_error =0;
	return 0;
}

//Memory allocator
void *Mem_Alloc(int size)
{
	void* ptr = list_add(size);
	if(ptr == NULL)
	{
		m_error = E_NO_SPACE;
		return NULL;
	}
	else 
		return ptr;
}

int Mem_Free(void* ptr)
{
	if(ptr == NULL)
	{
		m_error = E_BAD_POINTER;
		return (-1);
	}

	int ret_free = list_free(ptr);
	if(ret_free == -1)
		m_error = E_BAD_POINTER;

	return ret_free;
}


void Mem_Dump()
{
list_dump();
}
