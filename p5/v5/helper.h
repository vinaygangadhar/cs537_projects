#ifndef __helper_h__
#define __helper_h__

#define MAX_INODES (4096)
#define IMAP_SIZE	 (16)
#define MAX_IMAPS (256)

#define	MAX_DIRECT_P (14) 
#define MAX_DIR_ENTRIES (64)

#define MFS_DIRECTORY    (0)
#define MFS_REGULAR_FILE (1)
#define MFS_BLOCK_SIZE   (4096)

#define MSG_BUFFER_SIZE (8192)    //Redefined so that 4096 bytes + other integer arguments can be sent

#define DEBUG
#include "udp.h"

typedef unsigned int uint;
int disk_fd;   //Disk FD 

//Server useful struct
typedef struct __packet_t
{
	int sd;   												 //Socket descriptor to communicate
	struct sockaddr_in addr;					// Socket address
	char message[MSG_BUFFER_SIZE];			 //Message to send
	char buffer[MSG_BUFFER_SIZE];				//Recieved message
	int send_ack;								   //Send ack to track the sent msgs
}packet_t;

//Checkpoint Region
typedef struct __CR_t
{
	uint log_end;
	uint imap_p[MAX_IMAPS];
}CR_t;

//Imap definition
typedef struct __imap_t
{
	uint inode_p[IMAP_SIZE]; //16 inode pointers
}imap_t;

//Inode definition
typedef struct __Inode_t
{
	int size;
	int type;        //Regular or directory
	uint dp[MAX_DIRECT_P]; //14 direct pointers
}inode_t;

//Our own version of Directory entry
typedef struct __DirEnt_t {
		char name[60];  // up to 60 bytes of name in directory (including \0)
		int  inum;      // inode number of entry (-1 means entry not used)
} DirEnt_t;

//Directory DB - 4096B, 64 entries of 64B each
typedef struct __Dir_t
{
	DirEnt_t entry[MAX_DIR_ENTRIES];
}Dir_t;


//Server side Stat structure
typedef struct __S_Stat_t {
	    int type;   // MFS_DIRECTORY or MFS_REGULAR
			int size;   // bytes
			// note: no permissions, access times, etc.
}S_Stat_t;


//Client communication utilties
int send_msg();
int recv_msg();

//Server side utility functions
int send_resp(int, struct sockaddr_in*, char*);
int create_img(char*);

int get_cr(CR_t*);
int get_imap(int, imap_t*);
int get_inode(int , inode_t*);
int db_read(int, Dir_t*);
int CR_to_Inode(int, CR_t*, imap_t*, inode_t*);

//Server side library implemenation routines
int S_Stat(int, S_Stat_t*);
int S_Lookup(int, char*);
int S_Read(int, char (*)[MFS_BLOCK_SIZE], int, int*, int*);
int S_Write(int, char*, int);
int S_Creat(int, int, char*);
int S_Unlink(int, char*);

#endif //__helper_h__
