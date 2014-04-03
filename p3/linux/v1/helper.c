#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "helper.h"
#include <math.h>
#define DEBUG

void* list_add (int size){
	//align the size to be multiple of 8
	size = ((ceil)(size/8)) * 8 + sizeof(header_t);
#ifdef DEBUG
	printf("Allocated memory (MEM-ALLOC)= %d", size);
#endif
	header_t* new_node;
	node_t *temp =NULL, *found =NULL , *prev =NULL, *prev_temp=NULL;
	int low_size = 0;

	temp = head;
	do
	{
		if(temp->size > size){
			if(found == NULL){
				low_size = temp->size - size;
				found = temp;
				prev = prev_temp;
			}
			else {
				if(temp->size -size < low_size){
					found = temp;
					prev = prev_temp;
					low_size = temp->size -size;
				}
			}
		}
		temp = temp->next;
		prev_temp = temp;
	}while (temp != NULL);

	if(found == NULL)
		return NULL;
	else{
		if(low_size ==0)
			prev -> next = found-> next;
		else{
			temp  = size + found;
			temp-> size = low_size;
			temp-> next = found->next;
		}
		
		new_node =(header_t*) found;
		new_node->size = size;
		new_node->magic = 314159;
		return (new_node + sizeof(header_t*));
	}
}

int 
list_free(void* ptr){
	header_t* hptr = (header_t*)(ptr - sizeof(header_t));
	if(hptr->magic != 314159)
		return -1;
	else{
		//coalesce if possible
		node_t *temp =NULL;

		temp = head;
		do
		{
			if( ((temp->size +(long int) temp) ==(long int) hptr) || (((long int)temp ==  hptr->size + (long int) hptr))) {
			temp-> size += hptr->size;
			break;
			}
		}while (temp != NULL);
		head->next = (node_t*)hptr;
		head = (node_t*)hptr;
		return 0;
	}
}

