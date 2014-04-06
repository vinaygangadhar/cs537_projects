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
 	//printf("Entered List add");
	
	//Align the size to be multiple of 8 and add the header size to it
	int size = (int) (sz/8) ;
	size = size* 8 + sizeof(header_t);
	//#ifdef DEBUG
		printf("Allocated memory szie (MEM-ALLOC)= %d", size);
 	//#endif

	int diff_size = 0;
	header_t* new_header = NULL;
	node_t *cur_node = NULL, *found = NULL, *prev_node = NULL, *searched_node = NULL;
	cur_node = head;

	//List traversal
	do
	{
		if(cur_node->size >= size)
		{
			if(found == NULL)														//First node search, so found will be NULL
			{
				diff_size = cur_node->size - size;
				found = cur_node;
				prev_node = searched_node;
			}
			else if((cur_node->size - size) <= diff_size)
			{
					found = cur_node;
					diff_size = cur_node->size - size;
					prev_node = searched_node;
			}
		}

		searched_node = cur_node; 
		cur_node = cur_node->next;
	}while(cur_node->next!= NULL);

	//rejoin the links once the required chunk is found
	if(found == NULL)
		return NULL;
	else if(found == head)														//For head node chosen as the chunk to be returned
	{
		node_t* temp = head;
		if(diff_size == 0)
		{
			head = temp->next;
		}
		else
		{
			head = temp + size;
			head->size = diff_size;
			head->next = temp->next;
		}
	}
	else
	{
		if(diff_size == 0)
			prev_node->next = found->next;									//No new node to be created, just join the previous node to the next free node
		else
		{
			prev_node->next = found + size;									//Splitting done
			node_t* temp;																	//A temp node created here if the there is still some space in the chunk
			temp = found;
			temp->size = diff_size;												//Not sure a new node is created here	
			temp->next = searched_node;										
		}
	}

	//Calculations for header creation
	new_header = (header_t*) found;
	new_header->size = size;
	new_header->magic = MAGIC; 	
	
	//Calculate the node pointer to be returned
	found = found + sizeof(header_t);
	return ((void*)found);
}

int list_free(void* ptr)
{
	header_t* hptr = (header_t*) (ptr - sizeof(header_t));
	
	if(hptr->magic != MAGIC) //Check if its the valid pointer
		return (-1);
	
	node_t *cur_node = head;
	node_t* searched_node =NULL;

	//Deallocated node attached here - lot of corner cases not covered
	do
	{
		//if free space found before the freeing chunk
		if(((long int)cur_node + cur_node->size) == (long int)hptr)
		{
			cur_node->size += hptr->size;
			break;
		}
		
		//if free space found after the freeing chunk

		if(((long int)cur_node == (long int)hptr + hptr->size) && (searched_node !=NULL))
		{
			searched_node->next = (node_t*) hptr;
			hptr->size += cur_node->size;
		}
			
		searched_node = cur_node;
		cur_node = cur_node->next;
	}while(cur_node->next!= NULL);

	return 0;
}

void list_dump()
{
	node_t* cur_node;
	cur_node = head;

 	while(cur_node != NULL)
 	{
		printf("Start Addres:%p , Size:%d\n", cur_node, cur_node->size);
 		cur_node = cur_node->next;
	}
}	
