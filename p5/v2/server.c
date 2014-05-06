#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "udp.h"
#include "helper.h"

#define BUFFER_SIZE (4096)

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
		printf("Arguments received: Port = %d, File_image = %s\n", *port, *file_img);
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
				printf("File image doesn't exist - Creating a new one\n");
			#endif
			
			rfd =	create_img(file_img);
			if(rfd == -1)
			{
				#ifdef DEBUG
					printf("Error Creating the file - Exiting\n");
				#endif
				return -1;
			}
			//disk_fd = rfd;
		}

#ifdef DEBUG
		printf("File descriptor open: %d\n", disk_fd);
#endif

		//close(ofd);
		/*
		CR_t* cr;

		lseek(ofd, 0, SEEK_SET);
		int br = read(ofd, (void*)cr, 4);
		printf("BR: %d\n", br);	
		printf("Log end: %d\n", cr->log_end);
		printf("Imap_0 addr: %d\n", cr->imap_p[0]);
*/

		printf("                                SERVER:: waiting in loop\n");
		Stat_t* m =(Stat_t*) malloc(sizeof(Stat_t));
		sstat(10, m);
		printf("file->size=%d\n File->type=%d\n", m->size, m->type);	
    while (1) 
		{
			struct sockaddr_in s;
			char buffer[BUFFER_SIZE];
	
			int rc = UDP_Read(sd, &s, buffer, BUFFER_SIZE);
			
			if (rc > 0) 
			{
	    	printf("                                SERVER:: read %d bytes (message: '%s')\n", rc, buffer);
	    	char reply[BUFFER_SIZE];
	    	
				sprintf(reply, "reply");
				rc = UDP_Write(sd, &s, reply, BUFFER_SIZE);
			}
    }

    return 0;
}


