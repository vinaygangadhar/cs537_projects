#include "mfs.h"
#include "helper.h"

static packet_t com_t; 

//Send the message packet
int send_msg()
{
	int rc = UDP_Write(com_t.sd, &com_t.addr, com_t.message, MSG_BUFFER_SIZE);
	if(rc < 0)
		return -1;
	else
	{
		printf("				CLIENT:: Sent %d bytes (message: '%s')\n", rc, com_t.message);
		com_t.send_ack = 1;
	}

	return 0;
}

//Recieve the packet
int recv_msg() //TODO::Timeout functionality
{
	if(!com_t.send_ack)
		return -1;

		struct sockaddr_in raddr;
		memset(com_t.buffer, 0, MSG_BUFFER_SIZE); //Clear the recieving buffer
		
		int rc = UDP_Read(com_t.sd, &raddr, com_t.buffer, MSG_BUFFER_SIZE);
		if(rc < 0)
			return -1;
		else
			printf("				CLIENT:: Recieved %d bytes (message: '%s')\n", rc, com_t.buffer);
	
		return 0;
}

//Library routines
int MFS_Init(char* hostname, int port)
{
	int sd = UDP_Open(0);
	assert(sd > -1);

	struct sockaddr_in saddr;
	int rc = UDP_FillSockAddr(&saddr, hostname, port);
	assert(rc == 0);

	//Fill in the common com struct
	com_t.sd = sd;
	com_t.addr = saddr;		
	
#ifdef DEBUG
	printf("\n----Calling MFS_Init to server %s at port %d\n", hostname, port);
#endif

	printf("				CLIENT:: about to send the message (%d)\n", rc);

	return 0;
}

//Function 1 Send Format: Function_num,pinum,No_of_char_bytes,char_bytes
int MFS_Lookup(int pinum, char* name)
{
	memset(com_t.message, 0, MSG_BUFFER_SIZE); //Clear the send buffer
	
	//Provide the number of bytes to read for the name
	int len = strlen(name);
	snprintf(com_t.message, 3*sizeof(int), "%d%d%04d", 1, pinum, len); //Function 1 --LOOKUP
	strcat(com_t.message, name);

#ifdef DEBUG
	printf("\n----MFS_Lookup Sent message: %s\n", com_t.message);
#endif
	
	if(send_msg() < 0)
		return -1;

	//Recieve yet to write, should return inode num of name, if failure return -1
	if(recv_msg() < 0)
	{
		printf("					CLIENT:: Message was not recieved\n");
		return -1;
	}

#ifdef DEBUG
	printf("----MFS_Lookup Recieved message: %s\n", com_t.buffer);
#endif
	
	//Parse the response and return
	char buf[1];
	strncpy(buf, com_t.buffer, 1);
	if (atoi(buf) != 1)
		printf("Something went horribly wrong\n");

	strncpy(buf, com_t.buffer+sizeof(char), 1);
	int inum = atoi(buf);

#ifdef DEBUG
	printf("----MFS_Lookup- pinum: %d, name: %s == inum: %d\n", pinum, name, inum);
#endif

	return inum;
}

//Function 2 Send Format: Function_num,inum
int MFS_Stat(int inum, MFS_Stat_t* m)
{
	memset(com_t.message, 0, MSG_BUFFER_SIZE); //Clear the send buffer
	
	snprintf(com_t.message, 2*sizeof(int), "%d%d", 2, inum); //Function 2 -- MFS_Stat

#ifdef DEBUG
	printf("\n----MFS_Stat Sent message: %s\n", com_t.message);
#endif
	
	if(send_msg() < 0)
		return -1;

	//Once you recieve the response, fill in the MFS_Stat structure
	if(recv_msg() < 0)
	{
		printf("					CLIENT:: Message was not recieved\n");
		return -1;
	}

#ifdef DEBUG
	printf("----MFS_Stat Recieved message: %s\n", com_t.buffer);
#endif 

	//Parse the response and return
	char buf;
	char *pend;
	char bytes[4];

	//Func_id
	strncpy(&buf, com_t.buffer, 1);
	int func_id = strtol(&buf, &pend, 10);
	if (func_id != 2)
		printf("Something went horribly wrong\n");

	//Status
	strncpy(&buf, com_t.buffer+sizeof(char), 1);
	int status = atoi(&buf);

	//Type
	strncpy(&buf, com_t.buffer+(2*sizeof(char)), 1);
	m->type = atoi(&buf);

	//Type
	strcpy(bytes, com_t.buffer+(3*sizeof(char)));
	m->size = atoi(bytes);

#ifdef DEBUG
	printf("----MFS_Stat - inum: %d == Type: %d, Size: %d\n", status, m->type, m->size);
#endif

	return status;
}

//Function 3 Send Format: Function_num,inum,No_of_char_bytes,char_bytes,block
int MFS_Write(int inum, char* buffer, int block)
{
	memset(com_t.message, 0, MSG_BUFFER_SIZE); //Clear the send buffer

	int len = strlen(buffer);

	//Packing the block first before the write content
	snprintf(com_t.message, 4*sizeof(int), "%d%d%04d%d", 3, inum, len, block); //Function 3 -- MFS_Write
	strcat(com_t.message, buffer);

#ifdef DEBUG
	printf("\n----MFS_Write Sent message: %s\n", com_t.message);
#endif
	
	if(send_msg() < 0)
		return -1;

	//Once you recieve the response, response is write success or failure
	if(recv_msg() < 0)
	{
		printf("					CLIENT:: Message was not recieved\n");
		return -1;
	}

#ifdef DEBUG
	printf("----MFS_Write Recieved message: %s\n", com_t.buffer);
#endif 
	
	return 0;
}

//Function 4 Send Format: Function_num,block
int MFS_Read(int inum, char* buffer, int block)
{
	memset(com_t.message, 0, MSG_BUFFER_SIZE); //Clear the send buffer

	snprintf(com_t.message, 3*sizeof(int), "%d%d%d", 4, inum, block); //Function 4 -- MFS_Read

#ifdef DEBUG
	printf("\n----MFS_Read Sent message: %s\n", com_t.message);
#endif
	
	if(send_msg() < 0)
		return -1;

	//Once you recieve the response, response is written to buffer
	if(recv_msg() < 0)
	{
		printf("					CLIENT:: Message was not recieved\n");
		return -1;
	}

#ifdef DEBUG
	printf("----MFS_Read Recieved message: %s\n", com_t.buffer);
#endif

	return 0;
}

//Function 5 Send Format: Function_num,pinum,type,No_of_char_bytes,char_bytes
int MFS_Creat(int pinum, int type, char* name)
{
	memset(com_t.message, 0, MSG_BUFFER_SIZE); //Clear the send buffer

	int len = strlen(name);

	snprintf(com_t.message, 4*sizeof(int), "%d%d%d%04d", 5, pinum, type, len); //Function 5 -- MFS_Creat
	strcat(com_t.message, name);

#ifdef DEBUG
	printf("\n----MFS_Creat Sent message: %s\n", com_t.message);
#endif
	
	if(send_msg() < 0)
		return -1;

	//Once you recieve the response, status is returned
	if(recv_msg() < 0)
	{
		printf("					CLIENT:: Message was not recieved\n");
		return -1;
	}

#ifdef DEBUG
	printf("----MFS_Creat Recieved message: %s\n", com_t.buffer);
#endif
	
	return 0;
}

//Function 6 Send Format: Function_num,pinum,No_of_char_bytes,char_bytes
int MFS_Unlink(int pinum, char* name)
{
	memset(com_t.message, 0, MSG_BUFFER_SIZE); //Clear the send buffer

	int len = strlen(name);

	snprintf(com_t.message, 3*sizeof(int), "%d%d%04d", 6, pinum, len); //Function 6 -- MFS_Unlink
	strcat(com_t.message, name);

#ifdef DEBUG
	printf("\n----MFS_Unlink Sent message: %s\n", com_t.message);
#endif
	
	if(send_msg() < 0)
		return -1;

	//Once you recieve the response, status is returned
	
	return 0;
}

//Function 7 Send Format: Function_num,pinum
int MFS_Shutdown()
{
	memset(com_t.message, 0, MSG_BUFFER_SIZE); //Clear the send buffer

	snprintf(com_t.message, sizeof(int), "%d", 7); //Function 7 -- MFS_Shutdown

#ifdef DEBUG
	printf("\n----MFS_Shutdown Sent message: %s\n", com_t.message);
#endif
	
	if(send_msg() < 0)
		return -1;

	//Recieve a success status, and close client by calling exit(0)
	if(recv_msg() < 0)
	{
		printf("					CLIENT:: Message was not recieved\n");
		return -1;
	}

	//Parse the response and return
	char buf;
	char *pend1;

	strncpy(&buf, com_t.buffer, 1);
	int func_id = strtol(&buf, &pend1, 10);

	if (func_id != 7)
		printf("Something went horribly wrong: %d\n", atoi(&buf));
	
	
/*	
	char buf1, buf2;
	char *pend1 *pend2;

	strncpy(&buf1, com_t.buffer, 1);
	int func_id = strtol(&buf1, &pend1, 10);
	if (func_id != 7)
		printf("Something went horribly wrong\n");

	strncpy(&buf2, com_t.buffer+sizeof(char), 1);
	int status = atoi(&buf2);
*/

	int status = 0;

#ifdef DEBUG
	printf("----MFS_Shutdown Called - Exiting (%d)\n", status);
#endif

 return status;
}
