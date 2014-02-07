/* Program to sort the records based on 4 byte integer keys*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>
#include "sort.h"

//Usage Subroutine
void usage(char * pr)
{
	char * filename;
	filename = basename(pr);
	fprintf (stderr, "Usage: %s -i inputfile -o outputfile\n", filename);
	exit(1);
}

//Compare Subroutine
int cmpkeys (const void * a, const void * b)
{
	rec_t reca = *((rec_t*)a);
	rec_t recb = *((rec_t*)b);
	
	//printf("The keya is: %d\n", reca.key);
	//printf("The keyb is: %d\n", recb.key);

	return (reca.key - recb.key);
	//if ( reca.key <  recb.key ) return -1;
	//if ( reca.key ==  recb.key ) return 0;
	//if ( reca.key >  recb.key ) return 1;
}

//main program
int main (int argc, char ** argv)
{
	//Check the number of arguements
	if(argc !=5)
	{
//		fprintf (stderr, "Error: %s needs more arguements - See Usage\n", argv[0]);
		usage(argv[0]);
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
		//fprintf (stderr, "Error: Cannot open file %s\n", in_file);
		usage(argv[0]);
		exit(1);			
	}

	//Open the output file
	int out_fd = open(out_file, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
	if(out_fd < 0)
	{
		//fprintf (stderr, "Error: Cannot open file %s\n", out_file);
	  perror("open");
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
	unsigned long num_recs = bytes/sizeof(rec_t);
  
	//Allocate memory for recs
	rec_t * rec_ptr = (rec_t *) malloc(num_recs * sizeof(rec_t));
  if(rec_ptr == NULL)
	{
		fprintf(stderr, "Error: Cannot allocate memory for %lu records -> malloc failed\n", num_recs);
		exit(1);
	}

  //Read the input file
	rec_t line;
	rec_t * line_ptr = &line;
	int i = 0;	
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

  //  printf("Key: %u\n", line_ptr->key);
		if(i!=num_recs)
		{
   		rec_ptr[i] = *line_ptr; // rec_ptr[i] = *line_ptr 
			i++;
		}
	}
  close(in_fd);

	/*
	//test
	int k;
	int m;
	for (k = 0; k < num_recs; k++)
	{
		printf("Key: %d, recs: ", rec_ptr[k].key);
		for (m =0; m < 24; m++)
			printf("%d", rec_ptr[k].record[m]);
		printf("\n");
	}
*/

	//Sort Function - Read qsort manpage properly
	qsort(rec_ptr, num_recs, sizeof(rec_t), cmpkeys);

	printf("Number of records sorted: %lu\n", num_recs);

	//Write the sorted contents to a file
	int j = 0;
	for(j = 0; j < num_recs; j++)
	{
		int bw = write(out_fd, &rec_ptr[j], sizeof(rec_t));
		if(bw != sizeof(rec_t))
		{
			perror("write");
			exit(1);
		}
	}

	close(out_fd);

	//Free the allocated malloc pointer
	free(rec_ptr);

  return 0;
}

