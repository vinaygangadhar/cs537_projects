#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "pstat.h"
#include "param.h"

struct pstat p_stat;
struct pstat* pt = &p_stat;

int
spin() {
   while (1) {
      int i, j;
      for (i = 0; i < 10000000; i++) {
         j = i % 11;
      }
   }
}
int
main(int argc, char **argv)
{
	/*//printf(1,"getsyscallinfo is returning %d\n", getsyscallinfo()); //User program calling system call
	int i;
	for(i=-2; i<10; i+=1)
	{
	//	printf(1,"Set tickets is returning %d as and we are setting %d\n", settickets(i),i); 
		  //printf(1,"Ticket was set to %d\n")i
			//
			getpinfo(pt); 
	}
	*/

   int pids[10];
	 int i,r,j;
	 int ppid = getpid();
		printf(1,"Setting tickets:");
   for(i=0; i<10; i++)
	 {
		pids[i] = fork();
		if (pids[i] == 0) 
		{
      r = settickets(i*10+10);
			printf(1,"%d ",(i*10+10));
      if (r != 0) 
			{
         printf(1, "settickets failed");
         kill(ppid);
      }
      spin();
		}
	 }
	 
		printf(1,"Wait started");
	 sleep(100);
		printf(1,"Wait ended");

	 int lticks[10] ;
	 int hticks[10];
   struct pstat st;

   for(i = 0; i < NPROC; i++) {
      for(j = 0; j < 10; j++) {
         if (st.inuse[i] && st.pid[i] == pids[j]) {
            lticks[j] = st.lticks[i];
            hticks[j] = st.hticks[i];
						printf(1," %d\t %d\n",lticks[j], hticks[j]);
         }
      }
   }

   for (i = 0; i < 10; i++) {
      kill(pids[i]);
      wait();
   }
	 exit();
}
