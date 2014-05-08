#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>

#include "helper.h"
#include "udp.h"

static int disk_fd;

//Create a file system image and initialize the root directory
int create_img(char* file_path)
{
	disk_fd = open(file_path, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if(disk_fd == -1)
		printf("Open failed in %s\n", __FUNCTION__);

	//initialize with a CR, imap, rot dir and 2 entries
	uint disk_start_addr, cur_offset;

	disk_start_addr = 0x00000000;

	//Root dir
	cur_offset = disk_start_addr + sizeof(CR_t); 

	const char* cur_dir = ".";
	const char* par_dir = "..";
	
	Dir_t root;
	strcpy(root.entry[0].name, cur_dir);
	root.entry[0].inum = 0;
	strcpy(root.entry[1].name, par_dir);
	root.entry[1].inum = 0;
	
	//Remaining of the root dir entries inum should be initialised to -1
	int i;

	for(i = 2; i < 64; i++)
	{
		root.entry[i].inum = -1;
	}

	//Create an inode for the root
	inode_t r_inode;
	r_inode.type = MFS_DIRECTORY;
	r_inode.dp[0] = cur_offset; 													//Point in to the 1st DB of dir
	r_inode.size = MFS_BLOCK_SIZE;											 //Should be the last byte of the root dir

	cur_offset += sizeof(Dir_t); 
	//Update the imap
	imap_t imap_0;
	imap_0.inode_p[0] = cur_offset; 									//1st inode is the root inode
	for (i = 1; i < IMAP_SIZE; i++)
		imap_0.inode_p[i] = 0;


	cur_offset +=  sizeof(inode_t); 
	//Update the CR
	CR_t cr;
	cr.imap_p[0] = cur_offset;
	for (i = 1; i < MAX_IMAPS; i++)
		cr.imap_p[i] = 0;

	cur_offset += sizeof(imap_t);
	cr.log_end = cur_offset;

#ifdef DEBUG
	printf("CR addr: %d, Root addr: %d, Inode addr: %d, Imap Addr: %d, Log end: %d\n", disk_start_addr, r_inode.dp[0], imap_0.inode_p[0], cr.imap_p[0], cr.log_end);
#endif

	//Write the segment to the image
	int bw;
	//char buf[4096];

	lseek(disk_fd, 0, SEEK_SET);

	CR_t* crp = &cr;
	inode_t* ip = &r_inode;
	imap_t* imp = &imap_0;
	Dir_t*  rp = &root;

	bw = write(disk_fd, crp, sizeof(CR_t));
	if(bw != sizeof(CR_t))
		perror("write");

#ifdef DEBUG
	printf("CR Region written: %d\n", bw);
#endif

	bw = write(disk_fd, rp, sizeof(Dir_t));
	if(bw != sizeof(Dir_t) )
		perror("write");

#ifdef DEBUG
	printf("Root Dir written: %d\n", bw);
#endif

	bw = write(disk_fd, ip, sizeof(inode_t));
	if(bw != sizeof(inode_t) )
		perror("write");
	
#ifdef DEBUG
	printf("Inode written: %d\n", bw);
#endif
	
	bw = write(disk_fd, imp, sizeof(imap_t));
	if(bw != sizeof(imap_t) )
		perror("write");
		
#ifdef DEBUG
	printf("Imap written: %d\n", bw);
#endif

	fsync(disk_fd);
 
	return disk_fd;
}

