#include "mem.h"
#include "helper.h"

int main(int argc, char** argv)
{
	Mem_Init(4000);
	int* a= (int *)Mem_alloc(sizeof(int));
	a=10;
	printf("a=%d, %p", *a, a);
	Mem_Free(a);
	return 0;
}
