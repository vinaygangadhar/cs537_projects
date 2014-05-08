#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mfs.h"
#include "parser.h"

int main()
{
	char* msg = "120009hello.txt";
	char* msg1 = "340044A random long text to write into write buffer6";
	char* msg2 = "234";
	char* msg3 = "5210009hello.txt";
	int inum, block, type;
	char name[NAME_SIZE];
	char buffer[BUFFER_SIZE];

	parse_msg("1hello.txt");
	
	Lookup_parse(msg, &inum, &name);
	Stat_parse(msg, &inum);
	Write_parse(msg1, &inum, &buffer, &block);
	Read_parse(msg2, &inum, &block);
	Creat_parse(msg3, &inum, &type, &name);
	Unlink_parse(msg, &inum, &name);

	return 0;
}
