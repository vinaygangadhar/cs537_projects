#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#include "mem.h"
#include "helper.h"
#define MAGIC (314159)
#define DEBUG

void* list_add (int sz)
{
	list_dump();
	//Align the size to be multiple of 8 and add the header size to it
	int size = (int)(sz / 8);
	if((sz % 8) != 0)
		size++; 
	
	size = size * 8 + sizeof(header_t);

#ifdef DEBUG
	printf("\n");
	printf("Allocated memory size (MEM-ALLOC): %d\n", size);
#endif

	header_t* new_header = NULL;
	node_t* found = NULL, *searched_node = NULL, *prev_node = NULL;
	node_t* cur_node = head;

	//List traversal
	while(cur_node != NULL)
	{
		if(cur_node->size >= size)
		{
			#ifdef DEBUG
				printf("Best-Fit Loop: %p size is more than %d\n", cur_node, size);
			#endif
			if(found == NULL)														//First node search, so found will be head
			{
				#ifdef DEBUG
					printf("Best-Fit Loop: %p is first to satisfy size criteria \n", cur_node);
				#endif

				found = cur_node;
				prev_node = searched_node;
			}
			else if((cur_node->size) < (found->size))
			{
       	#ifdef DEBUG
					printf("Best-Fit Loop: %p size(%d) is less than previously found node size(%d) \n", cur_node, cur_node->size, found->size);
				#endif
				found = cur_node;
				prev_node = searched_node;
			}
		}

		searched_node = cur_node; 
		cur_node = cur_node->next;
	}

	//Rejoin the links once the required chunk is found
	if(found == NULL)
	{
		m_error = E_NO_SPACE;
		return NULL;
	}
	else if(found == head)														//For head node chosen as the chunk to be returned
	{
		node_t* temp = head;
		if(found->size == size || found->size == size + sizeof(node_t))
		{
			head = temp->next;
		}
		else
		{
			head = (node_t*)((long int) temp + size);
			head->size = found->size - size;
			head->next = temp->next;
		}
	}
	else
	{
		if(found->size == size || found->size==size + sizeof(node_t))
			prev_node->next = found->next;									//No new node to be created, just join the previous node to the next free node
		else
		{
			#ifdef DEBUG
				printf("Splitting started here: %p size(%d) is less than previously found node size(%d) \n", cur_node, cur_node->size, found->size);
			#endif

			/*prev_node->next =(node_t*)((long int)found + size);									//Splitting done
			node_t* temp;																		//A temp node created here if the there is still some space in the chunk
			temp = found;
			temp->size = found->size - size;*/								//Not sure a new node is created here	
			node_t* temp;																		//A temp node created here if the there is still some space in the chunk
			temp = (node_t*)((long int)found + size);	
			prev_node->next = temp;									//Splitting done
			temp->size = found->size - size;								//Not sure a new node is created here	
			temp->next = found->next;																			
		}
	}

	//Calculations for header creation
	new_header = (header_t*) found;
	if(found->size == size)
		new_header->size = size;
	else
		new_header->size = size + sizeof(node_t);
	new_header->magic = MAGIC; 	
	
	//Calculate the node pointer to be returned

#ifdef DEBUG
	printf("Returning from Mem_Alloc with hptr: %p, size: %d, head: %p\n", new_header, size, head);
#endif

	return ((void*)((long int)new_header + (long int) sizeof(header_t)));
}

int list_free(void* ptr)
{
	header_t* hptr = (header_t*) (ptr - sizeof(header_t));

#ifdef DEBUG
	printf("\n");	
	printf("Entering Mem_Free with hptr: %p, magic: %d, size: %d\n", hptr, hptr->magic, hptr->size);
#endif
	
	if(hptr->magic != MAGIC) 					//Check if its the valid pointer
		return (-1);

	if(head == NULL)
	{
#ifdef DEBUG
		printf("NULL header - nothing to coalesce\n");				//Nothing to coalesce
#endif

		head = (node_t*) hptr;
		head->size = hptr->size;
		head->next= NULL;
		return 0;
	}

	int coalesced_a = 0, coalesced_b = 0;
	node_t* cur_node = head;
	node_t* searched_node = NULL;

	//Deallocated node attached here - lot of corner cases not covered
  while(cur_node != NULL)	
	{

#ifdef DEBUG
	printf("Cur_node: %p\n",cur_node);
#endif

		//if free node found before the chunk being freed
		if(((long int)cur_node + cur_node->size) == (long int) hptr)
		{

#ifdef DEBUG
		printf("Free list node found before the chunk being freed\n");
#endif
			
			hptr->magic = 0;
			cur_node->size += hptr->size;
		//	hptr = (header_t*)cur_node;											//DONT KNOW Y U HAVE PUT THIS
			coalesced_a = 1;
		}
		
		//if free space found after the freeing chunk
		else if(((long int)cur_node == (long int)hptr + hptr->size))
		{
			if(cur_node == head)
			{
#ifdef DEBUG
		printf("Current node is Head\n");
#endif
				node_t* temp = head;
				head = (node_t*)hptr;
				head->next = temp->next;
				head->size = hptr->size + temp->size;
			}
			else
			{ //if head is not the current node, we need to change the previous node
				searched_node->next = (node_t*)hptr;
			}

			//hptr->size += cur_node->size;

#ifdef DEBUG
	printf("Free list node found after the chunk being freed - so change the head\n");
#endif
			coalesced_b =1;
		}
		
		searched_node = cur_node;
		cur_node = cur_node->next;

#ifdef DEBUG
		printf("Current Node = %p, Next node = %p\n", searched_node, cur_node);
#endif
	}

	if(coalesced_a== 0 && coalesced_b ==0)
		{
#ifdef DEBUG
	printf("Could not coalesce- Creating a new node\n");
#endif
		
			node_t* temp = head;
			head = (node_t*)hptr ;
			head->next = temp;
			head->size = hptr->size;
		}
	
		return 0;
}

void list_dump()
{
	node_t* cur_node;
	cur_node = head;
	int n = 0;
 	
	printf("\n-----------------------Mem---Dump---------------------------------------------\n");
	while(cur_node != NULL)
 	{
		printf("Node: %d, Start Address: %p , Available size: %d\n", n, cur_node, cur_node->size);
		cur_node = cur_node->next;
		n++;
	}
 	printf("---------------------------------------------------------------------------------\n");
}	
