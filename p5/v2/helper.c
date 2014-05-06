#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>

#include "helper.h"

//#define DEBUG
//Create a file system image and initialize the root directory
int create_img(char* file_path)
{
	//int new_fd;
	disk_fd = open(file_path, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if(disk_fd == -1)
		printf("Open failed in %s\n", __FUNCTION__);

	//initialize with a CR, imap, rot dir and 2 entries
	uint disk_start_addr, cur_offset;

	disk_start_addr = 0x00000000;

	//Root dir
	cur_offset = disk_start_addr + sizeof(CR_t); 

//	const char* root_name  = "/";
	const char* cur_dir = ".";
	const char* par_dir = "..";
	
	Dir_t root;
	//strcpy(root.entry[0].name, root_name);
	//root.entry[0].inum = 0;
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
	r_inode.dp[0] = cur_offset; 								//Point in to the 1st DB of dir
	r_inode.size = MFS_BLOCK_SIZE;											 //Should be the last byte of the root dir

	cur_offset += sizeof(Dir_t); 
	//Update the imap
	imap_t imap_0;
	imap_0.inode_p[0] = cur_offset; //1st inode is the root inode
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

/*
 //Sanity check read
 lseek(new_fd, 0, SEEK_SET);

 int br;
 CR_t dum_cr;
 Dir_t dum_dir;
 inode_t dum_inode;
 imap_t dum_imap;

 br = read(new_fd, &dum_cr, sizeof(CR_t));
 printf("BR: %d\n", br);
 printf("log end: %d\n", dum_cr.log_end);
 printf("imap_0: %d\n", dum_cr.imap_p[0]);
*/	
 
 return disk_fd;
}

void
get_cr(CR_t* lkp_cr_addr){
	lseek(disk_fd, 0, SEEK_SET);
	int rd = read(disk_fd, lkp_cr_addr, sizeof(CR_t));
	if (rd!=sizeof(CR_t)){
		printf("Error reading CR of the image");
		exit(-1);
	}
}

void
get_imap(int seek_addr, imap_t* imap_addr){
	lseek(disk_fd, seek_addr, SEEK_SET);
	int rd = read(disk_fd, imap_addr, sizeof(imap_t));
	if (rd!=sizeof(imap_t)){
		printf("Error reading imap");
		exit(-1);
	}
}

void
get_inode(int seek_addr, inode_t* inode_addr){
	lseek(disk_fd, seek_addr, SEEK_SET);
	int rd = read(disk_fd, inode_addr, sizeof(inode_t));
	if (rd!=sizeof(inode_t)){
		printf("Error reading inode");
		exit(-1);
	}
}

//Read a data block to address given
void
datablock_read(int seek_addr, Dir_t* data_addr){
		lseek(disk_fd, seek_addr, SEEK_SET);
		int rd = read(disk_fd, data_addr, sizeof(Dir_t));
		if (rd!=sizeof(Dir_t)){
			printf("Error reading data block");
			exit(-1);
		}
}

void
get_CR_to_inode(int inum,inode_t* inode){
	int imap_num = (int) inum/16;
	int imap_off = inum % 16;
	CR_t lkp_cr;
	
	imap_t parent_imap;
	uint parent_imap_addr;
	uint parent_inode_addr;

	//Go to checkpoint region and read parent imap Address
	get_cr(&lkp_cr);
	parent_imap_addr = lkp_cr.imap_p[imap_num];

	//Go to parent imap and read parent inode address
	get_imap(parent_imap_addr, &parent_imap);
	parent_inode_addr = parent_imap.inode_p[imap_off];

	//Go to parent inode and get parent directory data blocks
	get_inode(parent_inode_addr, inode);	
}

int
lookup(int pinum, char* name){
		inode_t parent_inode;
		uint data_addr;
		
	//Get the inode 
	get_CR_to_inode(pinum, &parent_inode);
	
	//Go through all the 14 data blocks of inode to check for the file name
	Dir_t iter;
	int it;
	for(it=0; it<14; it++){
		data_addr = parent_inode.dp[it];
		//Read the data block #it
		datablock_read(data_addr,&iter);
		

		//Read each data entry in the directory data block and see if the file name is there
		int entrynum;
		for(entrynum=0; entrynum<64; entrynum++){
			DirEnt_t tempfile= iter.entry[entrynum];
			if(strcmp(tempfile.name, name) ==0){
					return tempfile.inum;
			}
		}
	}

	return -1;
}

int
sstat(int inum, Stat_t *m){
	inode_t inode;
	if(inum >=4096)
		return -1;
	get_CR_to_inode(inum, &inode);	
	m->type = inode.type; 
	m->size = inode.size;
	return 0;
}

int 
swrite(int inum, char *buffer, int block){
	inode_t inode;

	if(inum >=4096)
		return -1;
	if(block>13)
		return -1;
	get_CR_TO_inode(inum, &inode);
	if(inode.type == MFS_DIRECTORY)
		return -1;
	data_addr = parent_inode.dp[it];
	lseek(data_addr, SEEK_SET);
	write(disk_fd, buffer, 4096);

	return 0;
}

int 
sread(int inum, char *buffer, int block){
	inode_t inode;

	if(inum >=4096)
		return -1;
	if(block>13)
		return -1;
	get_CR_TO_inode(inum, &inode);
	data_addr = parent_inode.dp[it];
	lseek(data_addr, SEEK_SET);
	read(disk_fd, buffer, 4096);

	return 0;
}

int 
screat(int pinum, int type, char *name){
	inode_t parent_inode;

	if(inum >=4096)
		return -1;
	get_CR_TO_inode(inum, &inode);
	if(inode.type == MFS_DIRECTORY)
		return -1;
	data_addr = parent_inode.dp[it];
	lseek(data_addr, SEEK_SET);

	return 0;
}

int 
sunlink(int pinum, char *name){
	return 0;
}

int 
sshutdown(){
	return 0;
}

