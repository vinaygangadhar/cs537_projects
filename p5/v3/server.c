#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "udp.h"
#include "helper.h"
#include "parser.h"


//Response sending routine
int send_resp(int sd, struct sockaddr_in* s, char* response)
{
	int rc = UDP_Write(sd, s, response, MSG_BUFFER_SIZE);

#ifdef DEBUG
	printf("				SERVER:: sending %d bytes (message: '%s')\n", rc, response);
#endif

	return rc;
}

//Get the args
void get_args(int* port, char** file_img, int argc, char* argv[])
{
    if (argc != 3) {
			fprintf(stderr, "Usage: %s [portnum] [file-system-image]\n", argv[0]);
			exit(1);
    }
		
		*port = atoi(argv[1]);
		*file_img = argv[2];

		if(*port < 1200){
			fprintf(stderr, "Port number must be more than 1200\n");
			exit(1);
		}

#ifdef DEBUG
		printf("\n----Arguments received: Port = %d, File_image = %s\n", *port, *file_img);
#endif
}

int
main(int argc, char *argv[])
{
		
		int port;
		char* file_img;
		int rfd;

		//Get the arguments
		get_args(&port, &file_img, argc, argv);
		
    int sd = UDP_Open(port); //Creates a new socket and binds to a port to it
    assert(sd > -1);

		//Open the file image, if not present call file_create routine
		disk_fd = open(file_img, O_RDWR);
		if(disk_fd == -1)
		{
			#ifdef DEBUG
				printf("----File image doesn't exist - Creating a new one\n");
			#endif
			
			rfd =	create_img(file_img);
			if(rfd == -1)
			{
				#ifdef DEBUG
					printf("----Error Creating the file - Exiting\n");
				#endif
				return -1;
			}
			disk_fd = rfd;
		}

#ifdef DEBUG
		printf("----File descriptor open: %d\n", disk_fd);
#endif

		printf("                                SERVER:: waiting in loop\n");
		
    while (1) 
		{
			struct sockaddr_in s;
			char buffer[MSG_BUFFER_SIZE];					//Redefined the size as, it may recieve more than 4096 bytes
	
			int rc = UDP_Read(sd, &s, buffer, MSG_BUFFER_SIZE);
			
			if (rc > 0) 
			{
	    	printf("                            	SERVER:: Read %d bytes (message: '%s')\n", rc, buffer);
	 
				int ret_func = parse_msg(buffer);   //Parse the message
				
				//Common paramters across all routines
				int pinum, inum, type, block, entries;
				char name[NAME_SIZE];
				char rw_buf[MFS_BLOCK_SIZE];
				char buf[MSG_BUFFER_SIZE];
				int res_t, ret;  //res_t- return status of server side routines
				S_Stat_t m;

				switch(ret_func)
				{
					case 1:
						ret = Lookup_parse(buffer, &pinum, &name);
						if (!ret)
							res_t = S_Lookup(pinum, name);
						else
							res_t = -1;
						
						//MSG Format: Function_id, Success_state(-1 or inum)
						memset(buf, 0, MSG_BUFFER_SIZE);
						snprintf(buf, 2*sizeof(int) , "%d%d", ret_func, res_t);
						send_resp(sd, &s, buf);
						break;
					case 2:
						ret = Stat_parse(buffer, &inum);
						if(!ret)
							res_t = S_Stat(inum, &m);
						else
							res_t = -1;
						
						//MSG Format: Function_id, Success_State, File_type, File_size
						memset(buf, 0, MSG_BUFFER_SIZE);
						snprintf(buf, 4*sizeof(int) , "%d%d%d%04d", ret_func, res_t, m.type, m.size);
						send_resp(sd, &s, buf);
						break;
					case 3:
						ret = Write_parse(buffer, &inum, &rw_buf, &block);
						if(!ret)
							res_t = S_Write(inum, rw_buf, block);
						else
							res_t = -1;

						//MSG Format: Function_id, Success_state
						memset(buf, 0, MSG_BUFFER_SIZE);
						snprintf(buf, 2*sizeof(int) , "%d%d", ret_func, res_t);
						send_resp(sd, &s, buf);
						break;
					case 4:
						ret = Read_parse(buffer, &inum, &block);
						if(!ret)
							res_t = S_Read(inum, &rw_buf, block, &type, &entries);
						else
							res_t = -1;
						
						//MSG Format: Function_id, Success_state, File_type, Num_of_entries, 4K buffer bytes read
						memset(buf, 0, MSG_BUFFER_SIZE);
						snprintf(buf, 4*sizeof(int) , "%d%d%d%02d", ret_func, res_t, type, entries);
						strcat(buf, rw_buf);
						send_resp(sd, &s, buf);
						break;
					case 5:
						ret = Creat_parse(buffer, &pinum, &type, &name);
						if(!ret)
							res_t = S_Creat(pinum, type, name);
						else
							res_t = -1;
						
						//MSG Format: Function_id, Success_state
						memset(buf, 0, MSG_BUFFER_SIZE);
						snprintf(buf, 2*sizeof(int) , "%d%d", ret_func, res_t);
						send_resp(sd, &s, buf);
						break;
					case 6:
						ret = Unlink_parse(buffer, &pinum, &name);
						break;
					case 7:
#ifdef DEBUG
						printf("----MFS_Shutdown called\n");
#endif
						break;
					default:
#ifdef DEBUG
						printf("Invalid Function Id\n");
#endif
						ret = -1;
						break;
				}
				
			}														//rc > 0 loop

    }														//while loop

    return 0;
}


