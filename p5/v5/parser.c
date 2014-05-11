#include "parser.h"
#include "helper.h"

//Message parser
int parse_msg(char* message)
{
	//Parse the function identifier 
	char buf;
	strncpy(&buf, message, 1);
	int func_id = atoi(&buf);

#ifdef DEBUG	
	printf("----Func_Identifier :%d\n",func_id);	
#endif

	return func_id;
}

//MFS_Lookup parser
int Lookup_parse(char* message, int* pinum, char (*name)[NAME_SIZE])
{
	char  *pend1, *pend2 ;
	char bytes1[BYTE_SIZE], bytes2[BYTE_SIZE];
	
	//Parse the parent inum
	strncpy(bytes1, message+sizeof(char), BYTE_SIZE);
	*pinum = strtol(bytes1, &pend1, DEC);

	//Parse the number of bytes of name
	strncpy(bytes2, message+(5*sizeof(char)), BYTE_SIZE);
	int num = strtol(bytes2, &pend2, DEC);

	if(num > NAME_SIZE)
	{
		printf("----Error: Filename too long - %d\n", num);
		return -1;
	}

	//Copy the name from message to temp
	char temp[num];
	strncpy(temp, message+(9*sizeof(char)), num+1);
	strcpy((char*)name, temp);
	//*name = temp;

#ifdef DEBUG
	printf("----MFS_Lookup called		- pinum: %d, name: %s\n", *pinum, *name);
#endif

	return 0;
}

//MFS_Stat parser
int Stat_parse(char* message, int* inum)
{
	char  *pend1;
	char bytes[BYTE_SIZE];
	
	//Parse the inum
	strncpy(bytes, message+sizeof(char), BYTE_SIZE);
	*inum = strtol(bytes, &pend1, DEC);

#ifdef DEBUG
	printf("----MFS_Stat called		- inum: %d\n", *inum);
#endif

	return 0;
}

//MFS_Write parser
int Write_parse(char* message, int* inum, char (*buffer)[MFS_BLOCK_SIZE], int* block)
{
	char  *pend1, *pend2;
	char bytes1[BYTE_SIZE], bytes2[2];
	
	//Parse the inum
	strncpy(bytes1, message+sizeof(char), BYTE_SIZE);
	*inum = strtol(bytes1, &pend1, DEC);

	//Parse the block offset   -- Parsing it first before the write content
	strncpy(bytes2, message+(5*sizeof(char)), 2);
	*block = strtol(bytes2, &pend2, DEC);

/*
	//Parse the number of bytes of buffer content to write
	strncpy(bytes2, message+(5*sizeof(char)), BYTE_SIZE);
	int num = strtol(bytes2, &pend2, DEC);

	if(num > MFS_BLOCK_SIZE)
	{
		printf("----Error: Write buffer too big - %d\n", num);
		return -1;
	}

	char temp[num];
	strncpy(temp, message+(11*sizeof(char)), num+1);
	strcpy((char*)buffer, temp);
*/	

	//Copy the write_buffer from message to temp
	int i;
	for (i = 0; i < 4096; i++)
		(*buffer)[i] = message[7+i];

	
#ifdef DEBUG 
	printf("----MFS_Write called		- inum: %d, Write_buffer: %s, Block_offset: %d\n", *inum, *buffer, *block);
#endif

	return 0;
}


//MFS_Read parser
int Read_parse(char* message, int* inum, int* block)
{
	char  *pend1, *pend2;
	char bytes1[BYTE_SIZE], bytes2[2];
	
	//Parse the inum
	strncpy(bytes1, message+sizeof(char), BYTE_SIZE);
	*inum = strtol(bytes1, &pend1, DEC);

	//Parse the block offset
	strncpy(bytes2, message+(5*sizeof(char)), 2);
	*block = strtol(bytes2, &pend2, DEC);

#ifdef DEBUG
	printf("----MFS_Read called		- inum: %d, block_offset: %d\n", *inum, *block);
#endif

	return 0;
}

//MFS_Creat parser
int Creat_parse(char* message, int* pinum, int* type, char (*name)[NAME_SIZE])
{
	char  *pend1, *pend2;
	char bytes1[BYTE_SIZE], bytes2[2];

	//Parse the inum
	strncpy(bytes1, message+sizeof(char), BYTE_SIZE);
	*pinum = strtol(bytes1, &pend1, DEC);

	//Parse the file type
	char buf[1];
	strncpy(buf, message+(5*sizeof(char)), 1);
	*type = atoi(buf);
	
	//Parse the number of bytes of name string
	strncpy(bytes2, message+(6*sizeof(char)), 2);
	int num = strtol(bytes2, &pend2, DEC);

	if(num > NAME_SIZE)
	{
		printf("----Error: Filename too long\n");
		return -1;
	}

	//Copy the name from message to temp
	char temp[num];
	strncpy(temp, message+(8*sizeof(char)), num+1);
	strcpy((char*)name, temp);
	
#ifdef DEBUG
	printf("----MFS_Creat called		- pinum: %d, type: %d, name: %s\n", *pinum, *type, *name);
#endif

	return 0;
}

//MFS_Unlink parser
int Unlink_parse(char* message, int* pinum, char (*name)[NAME_SIZE])
{
	char *pend1, *pend2;
	char bytes1[BYTE_SIZE], bytes2[BYTE_SIZE];

	//Parse the inum
	strncpy(bytes1, message+sizeof(char), BYTE_SIZE);
	*pinum = strtol(bytes1, &pend1, DEC);

	//Parse the number of bytes of buffer content to write
	strncpy(bytes2, message+(5*sizeof(char)), BYTE_SIZE);
	int num = strtol(bytes2, &pend2, DEC);
	
	if(num > NAME_SIZE)
	{
		printf("----Error: Filename too long\n");
		return -1;
	}

	//Copy the name from message to temp
	char temp[num];
	strncpy(temp, message+(9*sizeof(char)), num+1);
	strcpy((char*)name, temp);
	
#ifdef DEBUG
	printf("----MFS_Unlink called		- pinum: %d, name: %s\n",*pinum, *name);
#endif

	return 0;
}

