#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mfs.h"

int main(int argc, char** argv)
{

 char buf[MFS_BLOCK_SIZE];
 //char write_buf[MFS_BLOCK_SIZE];
 
 int ret = MFS_Init("procyon.cs.wisc.edu", 1345);
 if(ret != 0)
		printf("MFS_Init error\n");

 //File write test
 char* write_buf = (char*) malloc(4096* sizeof(char));
 int i;
 for(i = 0; i < 2000; i++)
	 write_buf[i] = 'A';
 for(i = 2000; i < 4000; i++)
	 write_buf[i] = '\0';
 for(i = 4000; i < 4096; i++)
	 write_buf[i] = 'B';

 ret = MFS_Write(14, write_buf, 0);	
 if(ret < 0)
	 printf("MFS_Write Error\n");

 //File read test 
 ret = MFS_Read(14, buf, 0);	
 if(ret < 0)
	 printf("MFS_Read Error\n");
 
 /*
 //Dir read test
 ret = MFS_Read(0, buf, 0);	
 if(ret < 0)
	 printf("MFS_Read Error\n");
*/

 ret = MFS_Shutdown();	
 if(ret < 0)
	 printf("MFS_Shutdown Error\n");

 return 0;
}
