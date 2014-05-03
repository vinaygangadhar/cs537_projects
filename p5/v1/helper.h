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

#define DEBUG

typedef unsigned int uint;

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


//Function Declarations
extern int create_img(char* file_path);

#endif //__helper_h__
