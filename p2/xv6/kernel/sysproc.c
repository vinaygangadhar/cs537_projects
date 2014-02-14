#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "sysfunc.h"

uint sys_count = 0;

int
sys_fork(void)
{
	++sys_count;
  return fork();
}

int
sys_exit(void)
{
	++sys_count;
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
	++sys_count;
  return wait();
}

int
sys_kill(void)
{
	++sys_count;
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
	++sys_count;
  return proc->pid;
}

int
sys_sbrk(void)
{
	++sys_count;
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
	++sys_count;
  int n;
  uint ticks0;
  
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
	++sys_count;
  uint xticks;
  
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


int
sys_getsyscallinfo(void)
{
	return ++sys_count;
}
