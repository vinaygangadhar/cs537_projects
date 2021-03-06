#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "pstat.h"
#include "param.h"

#define N 10

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

	 struct p_info
	 {
		 int pids;
		 int tk;
	 };

	 struct p_info p_table[N];

	 int i,r,j,k;
	 int ppid = getpid();
  
	 for(i = 0; i < N; i++)
	 {
		p_table[i].pids = fork();
		if (p_table[i].pids == 0) 
		{
			p_table[i].tk = i * N + N;
      r = settickets(i * N + N);
      if (r != 0) 
			{
         printf(1, "settickets failed");
         kill(ppid);
      }
      spin();
		}
	 }
	 //sleep(10);

	 int lticks[10] ;
	 int hticks[10];
   struct pstat st;
	 getpinfo(&st);	

	
   for(k = 0; i < NPROC; k++)
	 {
      for(j = 0; j < N; j++)
			{
         if (st.inuse[i] && (st.pid[i] == p_table[j].pids)) {
					  lticks[j] = st.lticks[i];
            hticks[j] = st.hticks[i];
						printf(1,"pid: %d\t tickets: %d\t ltick: %d\t hticks: %d\n",p_table[j].pids, p_table[j].tk, hticks[j], lticks[j]);
         }
      }
   }

   for (i = 0; i < N; i++) {
      kill(p_table[i].pids);
      wait();
   }
	 exit();
}
