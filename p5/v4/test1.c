#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mfs.h"

int main(int argc, char** argv)
{

 char buf[MFS_BLOCK_SIZE];
 //char write_buf[MFS_BLOCK_SIZE];
 
 int ret = MFS_Init("mumble-38.cs.wisc.edu", 1345);
 if(ret != 0)
		printf("MFS_Init error\n");

 MFS_Stat_t mfs_s;
 ret = MFS_Stat(1, &mfs_s);
 if(ret != 0)
	 printf("MFS_Stat Error\n");

 ret = MFS_Lookup(2, "helloworld.c");	
 if(ret < 0)
	 printf("MFS_Lookup Error\n");

 //File write test
 //strcpy(write_buf, "A random textaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa adj;p asd;kad'\0\0\0text"); 
 
 char* write_bu;
 write_bu = "Abc\0\0\0text";
 ret = MFS_Write(4, write_bu, 0);	
 if(ret < 0)
	 printf("MFS_Write Error\n");

 //File read test 
 ret = MFS_Read(4, buf, 0);	
 if(ret < 0)
	 printf("MFS_Read Error\n");
 
 /*
 //Dir read test
 ret = MFS_Read(0, buf, 0);	
 if(ret < 0)
	 printf("MFS_Read Error\n");
*/

 //Already existing file
 ret = MFS_Creat(0, 1, "hello.txt");
 if(ret < 0)
	 printf("MFS_Creat Error\n");

 ret = MFS_Unlink(0, "hello.txt");
 if(ret < 0)
	 printf("MFS_Unlink Error\n");

 ret = MFS_Shutdown();	
 if(ret < 0)
	 printf("MFS_Shutdown Error\n");

 /*	
 //Dir read test
 ret = MFS_Read(0, buf, 0);	
 if(ret < 0)
	 printf("MFS_Read Error\n");
 
 //File read test 
 ret = MFS_Read(4, buf, 0);	
 if(ret < 0)
	 printf("MFS_Read Error\n");

 //long name test
 ret = MFS_Creat(1, 1, "veryveryveryveryveryveryveryveryveryverylengthyfilename1111111111111111111.txt");
 if(ret < 0)
	 printf("MFS_Creat Error\n");

 //Already existing file
 ret = MFS_Creat(0, 1, "hello.txt");
 if(ret < 0)
	 printf("MFS_Creat Error\n");

 ret = MFS_Creat(0, 1, "test.txt");
 if(ret < 0)
	 printf("MFS_Creat Error\n");
*/

 return 0;
}
