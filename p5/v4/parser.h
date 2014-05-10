#ifndef __PARSER_H_
#define __PARSER_H_

#define FUNC_CHAR (0)
#define DEC (10) 
#define BYTE_SIZE (4)
#define NAME_SIZE (60)
#define BLOCK_SIZE (4096)

//Functions
int parse_msg(char*);

int Lookup_parse(char*, int*, char (*)[NAME_SIZE]);
int Stat_parse(char*, int*);
int Write_parse(char*, int*, char (*)[BLOCK_SIZE], int*);
int Read_parse(char*, int*, int*);
int Creat_parse(char*, int*, int*, char (*)[NAME_SIZE]);
int Unlink_parse(char*, int*, char (*)[NAME_SIZE]);


//Response parsers
int Lookup_resp(char*, int*);

#endif //__PARSER_H


