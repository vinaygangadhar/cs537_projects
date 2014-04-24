#include "types.h"
#include "user.h"
#include "x86.h"

#define PGSIZE (4096)

//Thread Create
int
thread_create(void (*start_routine)(void*), void* arg)
{
	void *stack = malloc(PGSIZE*2);
   
	if((uint)stack % PGSIZE)
     stack = stack + (4096 - (uint)stack % PGSIZE);

	int clone_pid = clone(start_routine, arg, stack);
	
	return clone_pid;
}

//Thread Join
int
thread_join()
{
	void* stack;

	int join_pid = join(&stack);
	
	free(stack);
	return join_pid;
}


//Lock init
void
lock_init(lock_t* lk)
{
	lk->locked = 0; //Inialise the lock to be 0
}

//Lock acquire 
void
lock_acquire(lock_t* lk)
{
	while(xchg(&lk->locked, 1) != 0);
	//Spin
}

//Lock Release 
void
lock_release(lock_t* lk)
{
	lk->locked = 0;
}

