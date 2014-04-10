#ifndef __helper_h__
#define __helper_h_

//node struct
typedef struct __node_t {
	int size;
	struct __node_t* next;
}node_t;

//allocated header structure
typedef struct __header_t {
	int size;
	int magic;
}header_t;

node_t* head;

//list add
extern void* list_add (int size);

//list free
extern int list_free(void* ptr);

//Coelscer
extern void coalesce();

//List Count
extern int list_count();

//List Sort
extern void list_sort();

//List dump
extern void list_dump();
#endif
