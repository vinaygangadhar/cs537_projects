#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "udp.h"
#include "helper.h"
#include "parser.h"

static int disk_fd;

//Response sending routine
int send_resp(int sd, struct sockaddr_in s, char* response)
{
	char resp[MSG_SIZE];
	sprintf(resp, response);
	int rc = UDP_Write(sd, &s, resp, MSG_SIZE);

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
		printf("----Arguments received: Port = %d, File_image = %s\n", *port, *file_img);
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
			char buffer[MSG_SIZE];					//Redefined the size as, it may recieve more than 4096 bytes
	
			int rc = UDP_Read(sd, &s, buffer, MSG_SIZE);
			
			if (rc > 0) 
			{
	    	printf("                                \nSERVER:: Read %d bytes (message: '%s')\n", rc, buffer);
	 
				int ret_func = parse_msg(buffer);   //Parse the message
				
				//Common paramters across all routines
				int pinum, inum, type, block;
				char name[NAME_SIZE];
				char write_buf[BUFFER_SIZE];
				int res_c, ret;

				switch(ret_func)
				{
					case 1:
						ret = Lookup_parse(buffer, &pinum, &name);
						break;
					case 2:
						ret = Stat_parse(buffer, &inum);
						break;
					case 3:
						ret = Write_parse(buffer, &inum, &write_buf, &block);
						break;
					case 4:
						ret = Read_parse(buffer, &inum, &block);
						break;
					case 5:
						ret = Creat_parse(buffer, &pinum, &type, &name);
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
				
				if(ret < 0)
				{
					printf("Parse or Response Error: Exiting\n");
					exit(1);
				}
				else if(res_c < 0)
					printf("Response Sending Failed: Resend\n");
				
			}														//rc > 0 loop

    }														//while loop

    return 0;
}


