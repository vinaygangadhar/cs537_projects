#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "pstat.h"

struct pstat p_stat;
struct pstat* pt = &p_stat;


int
main(int argc, char **argv)
{
	//printf(1,"getsyscallinfo is returning %d\n", getsyscallinfo()); //User program calling system call
	int i;
	for(i=-2; i<10; i+=1)
	{
	//	printf(1,"Set tickets is returning %d as and we are setting %d\n", settickets(i),i); 
		  //printf(1,"Ticket was set to %d\n")i
			//
			getpinfo(pt); 
	}
		exit();
}
