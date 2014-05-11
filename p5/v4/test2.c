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

 //Do a look up on that created file
 ret = MFS_Lookup(0, "test.txt");
 if(ret != 0)
	 printf("MFS_Lookup Error\n");

 ret = MFS_Shutdown();	
 if(ret < 0)
	 printf("MFS_Shutdown Error\n");

 return 0;
}
