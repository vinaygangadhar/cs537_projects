#ifndef __PARSER_H_
#define __PARSER_H_

#define FUNC_CHAR (0)
#define DEC (10) 
#define BYTE_SIZE (4)
#define BUFFER_SIZE (4096)
#define NAME_SIZE (60)
#define MSG_SIZE (8192)    //Redefined so that 4096 bytes + other integer arguments can be sent

//Functions
int parse_msg(char*);

int Lookup_parse(char*, int*, char (*)[NAME_SIZE]);
int Stat_parse(char*, int*);
int Write_parse(char*, int*, char (*)[BUFFER_SIZE], int*);
int Read_parse(char*, int*, int*);
int Creat_parse(char*, int*, int*, char (*)[NAME_SIZE]);
int Unlink_parse(char*, int*, char (*)[NAME_SIZE]);

#endif //__PARSER_H


