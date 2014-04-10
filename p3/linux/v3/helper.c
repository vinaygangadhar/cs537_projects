#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#include "helper.h"
#define MAGIC (314159)
//#define DEBUG

void* list_add (int sz)
{
	//Align the size to be multiple of 8 and add the header size to it
	int size = (int)(sz / 8);
	if(sz % 8 != 0)
		size++; 
	
	size = size * 8 + sizeof(header_t);

	#ifdef DEBUG
		printf("\n");
		printf("Allocated memory size (MEM-ALLOC): %d\n", size);
 	#endif

	header_t* new_header = NULL;
	node_t* found = head, *searched_node = NULL;
	node_t* cur_node = NULL;
	node_t* prev_node = NULL; 
	cur_node = head;

	//List traversal
	while(cur_node != NULL)
	{
		if(cur_node->size >= size)
		{
			if(found == head)														//First node search, so found will be head
			{
				found = cur_node;
				prev_node = searched_node;
			}
			else if(cur_node->size < found->size)
			{
				found = cur_node;
				prev_node = searched_node;
			}
		}

		searched_node = cur_node; 
		cur_node = cur_node->next;
	}

	//rejoin the links once the required chunk is found
	if(found == NULL)
		return NULL;
	else if(found == head)														//For head node chosen as the chunk to be returned
	{
		node_t* temp = head;
		if(found->size == size)
		{
			head = temp->next;
		}
		else
		{
			head =(node_t*)((long int) temp + (long int)size);
			head->size = found->size - size;
			head->next = temp->next;
		}
	}
	else
	{
		if(found->size == size)
			prev_node->next = found->next;									//No new node to be created, just join the previous node to the next free node
		else
		{
			prev_node->next = found + size;									//Splitting done
			node_t* temp;																	//A temp node created here if the there is still some space in the chunk
			temp = found;
			temp->size = found->size - size;												//Not sure a new node is created here	
			temp->next = searched_node;										
		}
	}

	//Calculations for header creation
	new_header = (header_t*) found;
	new_header->size = size; 
	new_header->magic = MAGIC; 	
	
	//Calculate the node pointer to be returned
	#ifdef DEBUG
		printf("Returning from Mem_Alloc with %p size = %d\n\n", new_header, size);
 	#endif
	return ((void*)((long int)new_header + (long int) sizeof(header_t)));
}

int list_free(void* ptr)
{
	header_t* hptr = (header_t*) ((long int)ptr - (long int) sizeof(header_t));
	#ifdef DEBUG
		printf("\nEntering Mem_Free  with hptr= %p magic=%d\n", hptr,hptr->magic);
 	#endif

	if(hptr->magic != MAGIC) //Check if its the valid pointer
		return (-1);
	
	int coalesceda =0, coalescedb=0;
	node_t *cur_node = head;
	node_t* searched_node =NULL;

	//Deallocated node attached here - lot of corner cases not covered
  while(cur_node !=NULL)	
	{
		#ifdef DEBUG
		printf("Free iteration=%p\n",cur_node);
 	  #endif

		//if free node found before the freeing chunk
		if(((long int)cur_node + cur_node->size) == (long int)hptr)
		{
					#ifdef DEBUG
				printf("Node found before the free chunk\n");
 	  		#endif
			hptr->magic = 0;
			cur_node->size += hptr->size;
			hptr = (header_t*)cur_node;
			coalesceda = 1;
		}
		
		//if free space found after the freeing chunk
		else if(((long int)cur_node == (long int)hptr + hptr->size))
		{
			if(cur_node == head)
			{
				#ifdef DEBUG
				printf("Current node is Head\n");
 	  		#endif
				head = (node_t*)cur_node;
				head->next = cur_node->next;
			}
			else
			{ //if head is not the current node, we need to change the previous node
				searched_node->next = (node_t*)hptr;
			}
			hptr->size += cur_node->size;
			#ifdef DEBUG
				printf("Free space found after the chunk\n");
 	  		#endif
					coalescedb =1;
		}
		
		searched_node = cur_node;
		cur_node = cur_node->next;
		#ifdef DEBUG
				printf("Current Node = %p\n Next node = %p", searched_node, cur_node);
				fflush(stdout);
 	  		#endif
			
	}
	if(coalesceda== 0 && coalescedb ==0)
		{
			#ifdef DEBUG
			printf("Could not coalesce\n");
 	  	#endif
			head = (node_t*)hptr ;
			head->next =head;
		}
  #ifdef DEBUG
		printf("Exiting from the Mem_Free\n");
 	#endif
	

	return 0;
}

void list_dump()
{
	node_t* cur_node;
	cur_node = head;

 	while(cur_node != NULL)
 	{
		printf("\nStart Addres:%p , Size:%d\n", cur_node, cur_node->size);
 		cur_node = cur_node->next;
	}
}	
