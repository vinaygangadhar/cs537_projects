#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>

#include "helper.h"
#include "udp.h"
#include "parser.h"

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
	for(i = 2; i < MAX_DIR_ENTRIES; i++)
		root.entry[i].inum = -1;

	//Create an inode for the root
	inode_t r_inode;
	r_inode.type = MFS_DIRECTORY;
	r_inode.dp[0] = cur_offset; 													//Point in to the 1st DB of dir
	r_inode.size = MFS_BLOCK_SIZE;											 //Should be the last byte of the root dir
	for(i = 1; i < MAX_DIRECT_P; i++)										//Initialise other data pointers to 0
		r_inode.dp[i] = 0;



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

//Get the CR
int get_cr(CR_t* lkp_cr_addr)
{
	lseek(disk_fd, 0, SEEK_SET);
	
	int rd = read(disk_fd, lkp_cr_addr, sizeof(CR_t));

	if (rd != sizeof(CR_t))
	{
		printf("Error reading CR of the image FD: %d, Got: %d, Expected: %lu\n", disk_fd, rd, sizeof(CR_t));
		return -1;
	}

	return 0;
}

//Get the Imap info
int get_imap(int seek_off, imap_t* pimap)
{
	lseek(disk_fd, seek_off, SEEK_SET);					//Go to disk location with seek_off as offset
	
	int rd = read(disk_fd, pimap, sizeof(imap_t));
	
	if (rd != sizeof(imap_t))
	{
		printf("Error reading imap- Disk offset Addr: %d\n", seek_off);
		return -1;
	}
	
	return 0;
}

//Get the Inode info
int get_inode(int seek_off, inode_t* pinode)
{
	lseek(disk_fd, seek_off, SEEK_SET);
	
	int rd = read(disk_fd, pinode, sizeof(inode_t));
	if (rd != sizeof(inode_t))
	{
		printf("Error reading inode: Disk offset Addr: %d\n", seek_off);
		return -1;
	}
	return 0;
}

//Read a data block to address given
int db_read(int seek_off, Dir_t* pdir)
{
	lseek(disk_fd, seek_off, SEEK_SET);
	
	int rd = read(disk_fd, pdir, sizeof(Dir_t));
		
	if (rd != sizeof(Dir_t))
	{
		printf("Error reading the data block: Disk offset Addr: %d\n0", seek_off);
		return -1;
	}
	return 0;
}

//Traverse from CR to Inode
int CR_to_Inode(int inum, CR_t* disk_cr, imap_t* pimap, inode_t* pinode)
{
	int imap_num = (int) (inum / IMAP_SIZE);  //Imap entry in the CR
	int imap_off = inum % IMAP_SIZE;          //Entry in the imap gives the corresponding inode addr

	uint imap_addr;
	uint inode_addr;

	//Go to checkpoint region and read imap
	int ret = get_cr(disk_cr);
	if(ret < 0)
		return -1;

	imap_addr = disk_cr->imap_p[imap_num];
	if(imap_addr == 0)												//Trying to access Inavlid imap
	{
		printf("Error: Invalid Imap: %d\n", imap_num);
		return -1;
	}

#ifdef DEBUG
	printf("## CR log_end:	%d, Imap[%d]_Addr: %d\n", disk_cr->log_end, imap_num, imap_addr);
#endif

	//Go to the imap and read inode address
	ret = get_imap(imap_addr, pimap);
	if(ret < 0)
		return -1;

	inode_addr = pimap->inode_p[imap_off];
	if(inode_addr == 0)										//Trying to access Inavlid inode
	{
		printf("Error: Invalid Inode: %d\n", inum);
		return -1;
	}

#ifdef DEBUG
	printf("## Imap[%d]:  inode_p[%d]_Addr: %d\n", imap_num, imap_off, inode_addr);
#endif

	//Go to inode and get the info
	ret = get_inode(inode_addr, pinode);	
	if(ret < 0)
		return -1;

#ifdef DEBUG
	printf("## Inode_%d:  Type: %d, Size: %d\n", imap_off, pinode->type, pinode->size);
#endif
	
	return 0;
}

//Server Side MFS_Lookup
int S_Lookup(int pinum, char* name)
{
	CR_t CR;
	imap_t imap;
	inode_t parent_inode;
	int found_inum;
	int found = 0;

	uint db_addr;

	if(pinum >= MAX_INODES)
		return -1;

	//Get the inode -- Gives the info about parent inode(directory) 
	int ret = CR_to_Inode(pinum, &CR, &imap, &parent_inode);
	if(ret < 0)
	{
		printf("----S_Lookup Failed\n");
		return -1;
	}

	//Check if the inode is directory type
	if(parent_inode.type != MFS_DIRECTORY)
	{
		printf("Error: Inode %d should be a Directory Inode\n", pinum);
		return -1;
	}

	//Go through all the 14 data blocks of inode to check for the file name
	Dir_t DB_Iter;
	
	int it;
	for(it = 0; it < MAX_DIRECT_P; it++)
	{
		db_addr = parent_inode.dp[it];  //Need to check if the db_addr is valid

		//Read the data block #it
		ret = db_read(db_addr, &DB_Iter);
		if(ret < 0)
			return -1;

		//Read each data entry in the directory data block and see if the file name is there
		int entrynum;
		for(entrynum = 0; entrynum < MAX_DIR_ENTRIES; entrynum++)
		{
			DirEnt_t temp_entry = DB_Iter.entry[entrynum];
			if((strcmp(temp_entry.name, name) == 0) && (temp_entry.inum != -1))
			{
				found = 1;
				found_inum = temp_entry.inum;
			}
		}
	}

	if(!found)
	{
		printf("Error: Name %s not found in dir %d\n", name, pinum);
		return -1;
	}

	fsync(disk_fd);
	return found_inum;
}


//Server Side MFS_Stat
int S_Stat(int inum, S_Stat_t* m){
	CR_t CR;
	imap_t imap;
	inode_t inode;

	if(inum >= MAX_INODES) //Max 4096 inodes
		return -1;

	int ret = CR_to_Inode(inum, &CR, &imap, &inode);
	if(ret < 0)
	{
		printf("----S_Stat FAILED\n");
		return -1;
	}

	//Fill in the stat structure
	m->type = inode.type; 
	m->size = inode.size;

#ifdef DEBUG
	printf("----S_Stat: inum: %d, type: %d, size: %d\n", inum, m->type, m->size);
#endif
	
	fsync(disk_fd);

	return 0;
}

//Server Side MFS_Write   //TODO: Update the file's size if new DB is installed , or retain if old DB is updated ??? -- Checking at line 365
int S_Write(int inum, char *buffer, int block)
{
	CR_t CR;
	imap_t imap_old, imap_new;
	inode_t inode_old, inode_new;

	int imap_num = (int)(inum / IMAP_SIZE);
	int imap_off = inum % IMAP_SIZE;
	int bw;

	if(inum >= MAX_INODES)
		return -1;
	if(block >= MAX_DIRECT_P)
		return -1;

	//Get the old CR, old imap, old inode
	int ret = CR_to_Inode(inum, &CR, &imap_old, &inode_old);
	if(ret < 0)
	{
		printf("----S_Write FAILED\n");
		return -1;
	}

	//If the file type is DIR 
	if(inode_old.type == MFS_DIRECTORY)
		return -1;
	
	//Write data block to end of log of CR
	uint cur_addr = CR.log_end;

	lseek(disk_fd, cur_addr, SEEK_SET);
	bw =  write(disk_fd, buffer, MFS_BLOCK_SIZE);
	if(bw != sizeof(MFS_BLOCK_SIZE))
		perror("write");

	//Update old inode and write it to new location
	//Also update the file's size if new DB is installed , or retain if old DB is updated ???
	inode_new = inode_old;
	if(inode_new.dp[block] == 0)
		inode_new.size += sizeof(MFS_BLOCK_SIZE);

	inode_new.dp[block] = cur_addr;					//Update the dp
	
	cur_addr += sizeof(Dir_t);

	//lseek(disk_fd, cur_addr, SEEK_SET);
	bw = write(disk_fd, &inode_new, sizeof(inode_t));
	if(bw != sizeof(inode_t))
		perror("write");

	//Update the imap
	imap_new = imap_old;
	imap_new.inode_p[imap_off] = cur_addr;
	
	cur_addr +=  sizeof(inode_t); 
	//lseek(disk_fd, cur_addr, SEEK_SET);
	bw = write(disk_fd, &imap_new, sizeof(imap_t));
	if(bw != sizeof(imap_t))
		perror("write");

	//Update the CR
	CR.imap_p[imap_num] = cur_addr; 
		
	cur_addr +=  sizeof(imap_t); 
	CR.log_end = cur_addr;

	lseek(disk_fd, 0, SEEK_SET);				//Go to the start of the disk
	bw = write(disk_fd, &CR, sizeof(CR_t));
	if(bw != sizeof(CR_t))
		perror("write");

	fsync(disk_fd);
	return 0;
}

//Server Side MFS_Read //TODO: If reading from uninitialised block, send all 0's ???
int S_Read(int inum, char (*buffer)[MFS_BLOCK_SIZE], int block, int* type, int* entries)
{
	CR_t CR;
	imap_t imap;
	inode_t inode;
	char buf[MFS_BLOCK_SIZE];
	int count = 0;

	//Checks
	if(inum >= MAX_INODES)
		return -1;
	if(block >= MAX_DIRECT_P)
		return -1;

	int ret = CR_to_Inode(inum, &CR, &imap, &inode);
	if(ret < 0)
	{
		printf("----S_Read Failed\n");
		return -1;
	}

	uint db_addr = inode.dp[block];
	if(db_addr == 0)
	{
		printf("Error: Invalid block offset: %d\n", block);
		return -1;
	}

	*type = inode.type;   //To help with file type at reciever side
	
	if(inode.type == MFS_REGULAR_FILE)
	{
		lseek(disk_fd, db_addr, SEEK_SET);
		
		memset(buf, 0 , MFS_BLOCK_SIZE);
		int rc = read(disk_fd, buf, MFS_BLOCK_SIZE);
		if(rc != MFS_BLOCK_SIZE)
		{
			printf("Error reading the file- inum: %d, block_offset: %d\n", inum, block);
			return -1;
		}
		strcpy((char*)buffer, buf);
    *entries = 0;   //An invalid parse for file type

#ifdef DEBUG
		printf("----S_Read File Content Read: %s\n", (char*)buffer);
#endif
	}
	else if(inode.type == MFS_DIRECTORY)
	{	
		int rc, it;
		lseek(disk_fd, db_addr, SEEK_SET);
		DirEnt_t ent;	
		
		memset(buf, 0 , MFS_BLOCK_SIZE);
		for(it = 0; it < MAX_DIR_ENTRIES; it++)
		{
			rc = read(disk_fd, &ent, 64);

			char inum_c[4];
			strcat(buf, ent.name);
			sprintf(inum_c, "%d", ent.inum);
			strcat(buf, inum_c);

			//Also need to calculate num of entries, so that recieving side only that many entries should be parsed
			if(ent.inum != -1)
				count++;
		}	
		strcpy((char*)buffer, buf);
		*entries = count;

#ifdef DEBUG
		printf("----S_Read Dir Read- Entries: %d, Content: %s\n", *entries, (char*)buffer);
#endif
	}
	
	fsync(disk_fd);
	return 0;
}

//Server Side MFS_Creat
int S_Creat(int pinum, int type, char *name)
{
	CR_t CR;
	imap_t parent_imap;
	inode_t parent_inode;

	//Checks
	if(pinum >= MAX_INODES)
		return -1;

	//Get the parent inode
	int ret = CR_to_Inode(pinum, &CR, &parent_imap, &parent_inode);
	if(ret < 0)
	{
		printf("----S_Creat Failed\n");
		return -1;
	}

	//Error Checks
	//Parent Inode must be a directory
	if(parent_inode.type != MFS_DIRECTORY)
		return -1;

	//The name too long category will already be checked in parse function and -1 will be returned there

	// Once above 2 checks are done, Check if the name already exists, if yes return 0;
	//Go through all the 14 data blocks of inode to check for the file name
	Dir_t DB_Iter;
	
	int it;
	int found = 0;
	uint db_addr;
	for(it = 0; it < MAX_DIRECT_P; it++)
	{
		db_addr = parent_inode.dp[it];
		
		if(db_addr == 0)
			continue;

		//Read the data block #it
		ret = db_read(db_addr, &DB_Iter);
		if(ret < 0)
			return -1;

		//Read each data entry in the directory data block and see if the file name is there
		int entrynum;
		for(entrynum = 0; entrynum < MAX_DIR_ENTRIES; entrynum++)
		{
			DirEnt_t temp_entry = DB_Iter.entry[entrynum];
			if((strcmp(temp_entry.name, name) == 0) && (temp_entry.inum != -1))
			{
				found = 1;
			}
		}
	}

	if(found)
	{
#ifdef DEBUG
		printf("Filename %s already exists - Creat exiting\n", name);
#endif
		return 0;
	}


  //Check if there are free entries in the parent dir (Among 14 data blocks)	
	Dir_t DB_par;
	
	int iter, entryno;
	int found_new_db = 0;
	int found_entry = 0;
	uint pdb_addr;

	for(iter = 0; iter < MAX_DIRECT_P; iter++)
	{
		entryno = 0;
		pdb_addr = parent_inode.dp[iter];
	  if(pdb_addr == 0)												//Say you finish 1st DB and all entries there are full, 2nd one is the new one
		{
			found_new_db = 1;
			break;
		}

		//Read the data block #it
		ret = db_read(pdb_addr, &DB_par);
		if(ret < 0)
			return -1;

		//Read each data entry in the directory data block and see if the file name is there
		for(entryno = 0; entryno < MAX_DIR_ENTRIES; entryno++)
		{
			DirEnt_t t_entry = DB_par.entry[entryno];
			if(t_entry.inum == -1)
			{
				found_entry = 1;
				break;
			}
		}
		if(found_entry)
			break;
	}

	if(found_new_db || found_entry)
#ifdef DEBUG
	printf("Free dir entry found in DB %d and free entry: %d\n", iter, entryno);
#endif
	else
		return -1;   					// No free entry found
	

	// Once a free entry is found, check for a free inode in all imaps [Check for all 4096 inodes]
	int imap_ptr, ip;
	int inode_found = 0;
	int new_imap_found = 0;
	uint imap_addr;
	imap_t imap_iter; 

	for(imap_ptr = 0; imap_ptr < MAX_IMAPS; imap_ptr++)
	{
		ip = 0;
		imap_addr = CR.imap_p[imap_ptr];
		if(imap_addr == 0)									//If its a new imap, then the inode will be the first one
		{
			new_imap_found = 1;
			break;
		}
			
		ret = get_imap(imap_addr, &imap_iter);
		if(ret < 0)
			return -1;

		for(ip = 0; ip < IMAP_SIZE; ip++)   // traverse through all 16 entries
		{
			if(imap_iter.inode_p[ip] == 0)
			{	
				inode_found = 1;
				break;
			}
		}
		if(inode_found)
			break;
	}

	if(inode_found || new_imap_found)
	{
#ifdef DEBUG
	printf("Choose inode offset %d in imap %d for new file/dir - Overall Inode num: %d\n", ip, imap_ptr, (imap_ptr * IMAP_SIZE) + ip);
#endif
	}
	else
		return -1; 						//All 4096 are occupied

	//Calculate the overall inode num
	int new_inum = (imap_ptr * IMAP_SIZE) + ip;

	//Get the current log of the file
	uint cur_addr = CR.log_end;
	int bw = 0;

	//ALL CHECKS DONE - We have free entry in a dir DB
	//We have a free inode in an imap
	inode_t new_inode;	
	Dir_t new_dir;
	int dp_iter;

	if(type == MFS_REGULAR_FILE)	
	{
		new_inode.type = type;
		new_inode.size = 0;
		
		for(dp_iter = 0; dp_iter < MAX_DIRECT_P; dp_iter++)		//initialise all the data pointers to 0
			new_inode.dp[dp_iter] = 0;

		//Write the inode
		lseek(disk_fd, cur_addr, SEEK_SET);
		ret = write(disk_fd, &new_inode, sizeof(inode_t));
	  if(bw != sizeof(inode_t))
			perror("write");

#ifdef DEBUG
		printf("Inode of a file written to disk\n");
#endif

	}
	else if(type == MFS_DIRECTORY)
	{
	
		const char* cur_dir = ".";
		const char* par_dir = "..";
	
		strcpy(new_dir.entry[0].name, cur_dir);
		new_dir.entry[0].inum = new_inum;										//Cur inode num -- newly found
		strcpy(new_dir.entry[1].name, par_dir);
		new_dir.entry[1].inum = pinum;									// Parent inode num
	
		//Remaining of the new dir db entries inum should be initialised to -1
		int i;
		for(i = 2; i < MAX_DIR_ENTRIES; i++)
			new_dir.entry[i].inum = -1;

		//Write the Dir DB
		lseek(disk_fd, cur_addr, SEEK_SET);
		ret = write(disk_fd, &new_dir, sizeof(Dir_t));
	  if(bw != sizeof(Dir_t))
			perror("write");

		new_inode.type = type;
		new_inode.size = MFS_BLOCK_SIZE;
	
		new_inode.dp[0] = cur_addr;	

		for(dp_iter = 1; dp_iter < MAX_DIRECT_P; dp_iter++)		//initialise all the data pointers to 0
			new_inode.dp[dp_iter] = 0;

		cur_addr += sizeof(Dir_t);

		lseek(disk_fd, cur_addr, SEEK_SET);
		bw = write(disk_fd, &new_inode, sizeof(inode_t));
		if(bw != sizeof(inode_t))
			perror("error");
				
#ifdef DEBUG
		printf("Inode of a Directory written to disk\n");
#endif

	}

	//Update the imap to include the pointer to new inode
	//If its a new imap need to update, first entry is the inode	
	imap_t new_imap;
	
	//Still the old imap
	if(inode_found)
	{
		printf("Using the Old imap: %d and the inode entry: %d\n", imap_ptr, ip);
		new_imap = imap_iter;
	}
	else if(new_imap_found)
		printf("Using the New imap: %d and the inode entry: %d\n", imap_ptr, ip);

	new_imap.inode_p[ip] = cur_addr;

	//Remove ===
	printf("imap's cur addr: %d\n", cur_addr);
	printf("imap's %d entry  addr: %d\n", ip, new_imap.inode_p[ip]);
	
	//Initialise remaining inode ptrs in the new imap to 0
	int i;
	if(new_imap_found)
	{
		for (i = 1; i < IMAP_SIZE; i++)
		new_imap.inode_p[i] = 0;
	}


	cur_addr += sizeof(inode_t);
	
	lseek(disk_fd, cur_addr, SEEK_SET);
	bw = write(disk_fd, &new_imap, sizeof(imap_t));
	if(bw != sizeof(imap_t))
		perror("write");


	//Update the CR 
	CR.imap_p[imap_ptr] = cur_addr;

	cur_addr += sizeof(imap_t);

	//Update the parent data structures----------
	Dir_t new_pdir;
	
	//Still a non-empty db
	if(found_entry)
		new_pdir = DB_par;	

	strcpy(new_pdir.entry[entryno].name, name);
	new_pdir.entry[entryno].inum = new_inum;

	//remove =======
	printf("DB_par entry num 3's inode: %d and name: %s\n", DB_par.entry[3].inum, DB_par.entry[3].name);
	printf("New DB_dir entry num 3's inode: %d and name: %s\n", new_pdir.entry[3].inum, new_pdir.entry[3].name);
	
	printf("New DB_dir entry num %d's inode: %d and name: %s\n", entryno, new_pdir.entry[entryno].inum, new_pdir.entry[entryno].name);
	// =======

	//Initialise remaining data entries in the new DB to -1
	if(found_new_db)
	{
		for (i = 1; i < MAX_DIR_ENTRIES; i++)
		new_pdir.entry[i].inum = -1;
	}

	//Write the data block
	lseek(disk_fd, cur_addr, SEEK_SET);
	bw = write(disk_fd, &new_pdir, sizeof(Dir_t));
	if(bw != sizeof(Dir_t))
		perror("write");

	//========
	printf("Parent inodes' DB num: %d\n", iter);
	//=====

	//Update the parent inode
	parent_inode.dp[iter] = cur_addr;
	if(found_new_db)
		parent_inode.size += sizeof(Dir_t);


	cur_addr += sizeof(Dir_t);

	lseek(disk_fd, cur_addr, SEEK_SET);
	bw = write(disk_fd, &parent_inode, sizeof(inode_t));
	if(bw != sizeof(inode_t))
		perror("error");
	
	//Update the parent imap
	int pimap_off = pinum % 16;
	int pimap_num = (int) (pinum / 16);


	//If the parent imap is same as the one updated before copy it and then update it
	if(pimap_num == imap_ptr)
	{	
		printf("Parent imap and new file's imap same: %d\n", imap_ptr);
		parent_imap = new_imap; 
	}

	//remove ====
	printf("Pimap %dth addr: %d == New _imap %dth addr: %d\n", ip, parent_imap.inode_p[ip], ip, new_imap.inode_p[ip]);


	parent_imap.inode_p[pimap_off] = cur_addr;

	//Write pimap
	cur_addr += sizeof(inode_t);
	
	lseek(disk_fd, cur_addr, SEEK_SET);
	bw = write(disk_fd, &parent_imap, sizeof(imap_t));
	if(bw != sizeof(imap_t))
		perror("write");
	
	//Finally Update the CR and write
	CR.imap_p[pimap_num] = cur_addr;

	cur_addr += sizeof(imap_t);

	CR.log_end = cur_addr;

	lseek(disk_fd, 0, SEEK_SET);
	bw = write(disk_fd, &CR, sizeof(CR_t));
	if(bw != sizeof(CR_t))
		perror("write");
	
	fsync(disk_fd);
	return 0;
}

//Server Side MFS_Unlink
int S_Unlink(int pinum, char* name)
{
	printf("Pinum: %d, name: %s\n", pinum, name);
	return 0;
}
