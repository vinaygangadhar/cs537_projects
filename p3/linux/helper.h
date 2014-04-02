#ifndef __helper_h__
#define __helper_h_

//node struct
typedef struct __node_t {
	int size;
	struct __node_t* next;
}node_t;

//allocated header structure
typedef struct __header_t {
	int magic;
	int size;
}header_t;

node_t* head;

//list add
extern void* list_add (int size);

//list free
extern int list_free(void* ptr);

#endif
