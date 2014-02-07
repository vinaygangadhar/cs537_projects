/* Program to sort the records based on 4 byte integer keys*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "sort.h"

//Usage Subroutine
void usage(char * pr)
{
	fprintf (stderr, "Usage: %s -i input file -o output file\n", pr);
	exit(1);
}


//main program
int main (int argc, char ** argv)
{
	//Check the number of arguements
	if(argc < 2)
	{
		fprintf (stderr, "Error: %s needs 2 arguements - See Usage\n", argv[0]);
		exit(1);
	}

	//input arguments
	char * in_file = "random_file";
	char * out_file = "random_file";
	
	//Parse the arguments
	int r;
	while((r = getopt(argc, argv, "i:o:h")) !=-1)
	{
		switch(r) {
			case 'i':
				in_file = strdup(optarg);
				break;
			case 'o':
				out_file = strdup(optarg);
				break;
			case 'h':
			  usage(argv[0]);	
				break;
			default:
				usage(argv[0]);
		}
	}
	
	//Open the input file
	int in_fd = open(in_file, O_RDONLY, S_IRUSR);
	if(in_fd < 0)
	{
		fprintf (stderr, "Error: Cannot open file %s\n", in_file);
		//perror("open");
		exit(1);			
	}

	//Open the output file
	int out_fd = open(out_file, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
	if(out_fd < 0)
	{
		fprintf (stderr, "Error: Cannot open file %s\n", out_file);
		//perror("open");
		exit(1);			
	}
 
	//Get the size of input file 
 	struct stat buffer;
	struct stat * buf = &buffer;
	int status = fstat(in_fd, buf);
	if(status < 0)
	{
		fprintf(stderr, "Cannot read the contenst of %s file\n", in_file);
		exit(1);
	}
	off_t bytes = buf->st_size;
	unsigned long num_recs = bytes/100;
	printf("Number of records read: %lu\n", num_recs);


  //Read the input file
	rec_t line;
	rec_t * line_ptr = &line;
	
	while(1) //Can also implement using for loop since you know the input file size now
	{
		int br = read(in_fd, line_ptr, sizeof(rec_t));
		if (br < 0)
		{
			fprintf(stderr, "Error: Cannot read file %s\n", in_file);
			exit(1);
		}

  	if(br == 0)
		{
			break;  //Indicates the EOF and no bytes are read
		}

    printf("Key: %u\n", line_ptr->key);
	
		//Write the sorted contents to a file
		int bw = write(out_fd, line_ptr, sizeof(rec_t));
		if(bw != sizeof(rec_t))
		{
			perror("write");
			exit(1);
		}
	}

  close(in_fd);
	close(out_fd);

  return 0;
}

