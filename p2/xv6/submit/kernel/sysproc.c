#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "sysfunc.h"
#include "pstat.h"

int counter;
int
sys_fork(void)
{
	counter++;
  return fork();
}

int
sys_exit(void)
{
	counter++;
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
	counter++;
  return wait();
}

int
sys_kill(void)
{
	counter++;
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
	counter++;
  return proc->pid;
}

int
sys_sbrk(void)
{
	counter++;
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;
  
	counter++;
  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since boot.
int
sys_uptime(void)
{
  uint xticks;
  
	counter++;
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

//Gets number of times system call has been called
int 
sys_getsyscallinfo(void) //Anil: Definition of System call
{
	counter++;
	return counter;
}

//Set number of tickets are requested by the processor
int
sys_settickets(void)
{
	int num;
	if(argint(0, &num) < 0)
    return -1;

	if(num >= 1)
	{
		proc->tickets = num;
		return 0;
	}
	else
		return -1;
}


//Provides Porcessor info to the calling function
int
sys_getpinfo (void)
{
	struct pstat* pst;

	if(argptr(0,(void *) &pst, sizeof(*pst)) < 0)
		return -1;

	else
	{
		struct proc* p;
		int i = 0;

		for (p = &ptable.proc[0]; p < &ptable.proc[NPROC]; p++,i++)
		{
			if(p->state == SLEEPING || p->state == RUNNING || p->state == RUNNABLE)
			{
				pst->hticks[i] = p->htix;
				pst->lticks[i] = p->ltix;
				pst->pid[i] 	 = p->pid;
				pst->inuse[i]	 = 1;
			
			}
		}
		return 0;	
	}
}
