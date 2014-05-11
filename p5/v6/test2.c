#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mfs.h"

int main(int argc, char** argv)
{

 int ret = MFS_Init("procyon.cs.wisc.edu", 1345);
 if(ret != 0)
		printf("MFS_Init error\n");

 //long name test
 ret = MFS_Creat(1, 1, "veryveryveryveryveryveryveryveryveryverylengthyfilename1111111111111111111.txt");
 if(ret < 0)
	 printf("MFS_Creat Error\n");

 //Already existing file
 ret = MFS_Creat(0, 1, "hello.txt");
 if(ret < 0)
	 printf("MFS_Creat Error\n");

 //Actual create test (creating a file)
 ret = MFS_Creat(0, 1, "test.txt");
 if(ret < 0)
	 printf("MFS_Creat Error\n");

 //Actual create test (creating 1 more file )
 ret = MFS_Creat(0, 1, "new_inode_test.txt");
 if(ret < 0)
	 printf("MFS_Creat Error\n");

 //Actual create Dir test
 ret = MFS_Creat(0, 0, "new_dir");
 if(ret < 0)
	 printf("MFS_Creat Error\n");

 //Do a look up on that created file
 ret = MFS_Lookup(0, "test.txt");
 if(ret < 0)
	 printf("MFS_Lookup Error\n");
 else
	 printf("MFS_Lookup inum: %d\n", ret);

 //Do a look up on that 2nd file
 ret = MFS_Lookup(0, "new_inode_test.txt");
 if(ret < 0)
	 printf("MFS_Lookup Error\n");
 else
	 printf("MFS_Lookup inum: %d\n", ret);

 //Do a look up on that new dir
 ret = MFS_Lookup(0, "new_dir");
 if(ret < 0)
	 printf("MFS_Lookup Error\n");
 else
	 printf("MFS_Lookup inum: %d\n", ret);

 MFS_Stat_t mfs_s;
 ret = MFS_Stat(14, &mfs_s);
 if(ret <  0)
	 printf("MFS_Stat Error\n");
 else
	 printf("MFS Stat of inum %d- type: %d, size: %d\n", 14, mfs_s.type, mfs_s.size);

 ret = MFS_Stat(15, &mfs_s);
 if(ret <  0)
	 printf("MFS_Stat Error\n");
 else
	 printf("MFS Stat of inum %d- type: %d, size: %d\n", 15, mfs_s.type, mfs_s.size);

 //Unlink
 ret = MFS_Unlink(0, "test.txt");
 if(ret < 0)
	 printf("MFS_Unlink error");
 
 //Unlink dir
 ret = MFS_Unlink(0, "new_dir");
 if(ret < 0)
	 printf("MFS_Unlink error");
 
 ret = MFS_Shutdown();	
 if(ret < 0)
	 printf("MFS_Shutdown Error\n");

 return 0;
}
