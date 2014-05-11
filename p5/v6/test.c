#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mfs.h"

int main(int argc, char** argv)
{
 char buf[MFS_BLOCK_SIZE];
 int inum;

 int ret = MFS_Init("procyon.cs.wisc.edu", 1345);
 if(ret != 0)
		printf("MFS_Init error\n");

 ret = MFS_Lookup(1, "hello.txt");
 if(ret != 0)
		printf("MFS_Lookup error\n");

 MFS_Stat_t mfs_s;
 ret = MFS_Stat(1, &mfs_s);
 if(ret != 0)
	 printf("MFS_Stat error\n");

 strcpy(buf, "A random text to be written to write_buffer");
 ret = MFS_Write(1, buf, 4);
 if(ret != 0)
	 printf("MFS_Write error\n");

 ret = MFS_Read(1, buf, 4);
 if(ret != 0)
	 printf("MFS_Read error\n");


 inum = MFS_Creat(0, MFS_REGULAR_FILE, "sample.txt");
 if(inum != 0)
	 printf("MFS_Creat error\n");

 ret = MFS_Unlink(0, "hello.txt");
 if(ret != 0)
	 printf("MFS_Unlink error\n");

 ret = MFS_Shutdown();
 if(ret != 0)
	 printf("MFS_Shutdown error\n");
 
 return 0;
}
