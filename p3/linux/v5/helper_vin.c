#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#include "mem.h"
#include "helper.h"
#define MAGIC (314159)
//#define DEBUG

void* list_add (int sz)
{
	//Align the size to be multiple of 8 and add the header size to it
	int size = (int)(sz / 8);
	if((sz % 8) != 0)
		size++; 
	
	size = size * 8 + sizeof(header_t);

#ifdef DEBUG
	printf("\n------------------------------MEM-ALLOC-------------------------------\n");
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
			if(found == NULL)																			//First node search, so found will be head
			{
				found = cur_node;
				prev_node = searched_node;
			}
			else if(cur_node->size <= found->size)
			{
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
	else if(found == head)																		//For head node chosen as the chunk to be returned
	{
		node_t* temp = head;
		if(found->size == size)
		{
			head = temp->next;
		}
		else if(found->size <= size + sizeof(header_t))
		{
			head = temp->next;
			size = found->size;
		}
		else
		{
		/*
			if(size > head->size)
			{
				m_error = E_NO_SPACE;															//Check if remaining free list size has enough to allocate the request
				return NULL;
			}
		*/	
			head = (node_t*)((long int) temp + size);
			head->size = found->size - size;
			head->next = temp->next;
		}
	}
	else
	{
		if(found->size == size)
			prev_node->next = found->next;											//No new node to be created, just join the previous node to the next free node
		else if((found->size - size) <= sizeof(header_t))
		{
			prev_node->next = found->next;
			size = found->size;
		}
		else
		{
			node_t* temp;																			//A temp node created here if the there is still some space in the chunk
			temp =(node_t*)((long int)found + size);									//Splitting done
			prev_node->next = temp;	
			temp->size = found->size - size;		
			temp->next = found->next;										
		}
	}

	//Calculations for header creation
	new_header = (header_t*) found;
	new_header->size = size; 
	new_header->magic = MAGIC; 	
	
	//Calculate the node pointer to be returned

#ifdef DEBUG
	printf("Returning from Mem_Alloc with PTR: %p, HPTR: %p, Size: %d, HEAD: %p\n", (void*)((long int)new_header + sizeof(header_t)), new_header, size, head);
#endif

	return ((void*)((long int)new_header + (long int) sizeof(header_t)));
}

int list_free(void* ptr)
{
	header_t* hptr = (header_t*) (ptr - sizeof(header_t));

#ifdef DEBUG
	printf("\n------------------------------MEM-FREE--------------------------------\n");
	printf("Entering Mem_Free with PTR: %p, HPTR: %p, Magic: %d, Size: %d\n", ptr, hptr, hptr->magic, hptr->size);
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
	
	//Attach the new incoming ptr as the new head
	node_t* temp = head;
	head = (node_t*) hptr;
	head->size = hptr->size;
	head->next = temp;

	//Calling Coalescer
	coalesce();
	return 0;
}

void coalesce()
{
	//Before coaelscing Sort the linked list as a serial list with head pointing to lowest address
	list_sort();
	//list_dump();

	node_t* cur_node = head;
	node_t *next_node = NULL, *prev_node = NULL;

#ifdef DEBUG
	printf("<=====Coaelescer=====>\n");
#endif

	while(cur_node!= NULL)
	{
#ifdef DEBUG
		printf("Cur_node: %p\n",cur_node);
#endif
		
		next_node = cur_node->next;																		//Get the info about the next node in the list
		if(next_node != NULL)
		{
			if((long int)next_node == (long int)cur_node + cur_node->size)
			{
#ifdef DEBUG
		printf("Free list node is serially 'NEXT' to current_node - Coalesce\n");
#endif	
				cur_node->size += next_node->size;
				cur_node->next = next_node->next;
			}
			else if (prev_node!= NULL)
			{
				if((long int)cur_node == ((long int)prev_node + prev_node->size))
				{
#ifdef DEBUG
		printf("Free list node is serially 'PRIOR' to current_node - Change the head and Coalesce\n");
#endif
		/*
					node_t* temp = head;
					head = next_node;
					head->size = temp->size + next_node->size;
					head->next = next_node->next;
		*/
					prev_node->next = cur_node->next;
					prev_node->size += cur_node->size;
				}
			}
		}
		else																											// In case the last node is NULL check for coalescing only with previous free node
		{
			if((long int)cur_node == (long int)prev_node + prev_node->size)
			{
#ifdef DEBUG
		printf("Last Node : free list node is serially 'PRIOR' to current_node - Coalesce\n");
#endif
			prev_node->next = cur_node->next;
			prev_node->size += cur_node->size;
			}
		}
		
		prev_node = cur_node;
		cur_node = cur_node->next;
	}


}

void list_sort()
{
	int count = list_count();
	int i, j;
	node_t *cur = head, *nxt = NULL, *temp = NULL, *prev = NULL, *swapped = NULL;

	//list_dump();
	if(!head  || (head->next == NULL) )  											//A null list or a single node return
		return;

	for(i =0; i < count; i++)
	{
		for(j=0; (cur!= NULL) && (j < count-i-1); j++)
		{
			nxt = cur->next;
			if(nxt!= NULL)
			{
				if(cur > nxt)
				{
					if(cur == head)
					{
						temp = head;
						head = nxt;
						cur->next = nxt->next;
						nxt->next = temp;
						swapped = nxt;
					//	printf("head: %p, head->next: %p, cur: %p, cur->next: %p, swapped: %p\n", head, head->next, cur, cur->next, swapped);
					}
					else
					{
						temp = cur;
						cur->next = nxt->next;
						prev->next = nxt;
						swapped = nxt;
						nxt->next = temp;
					//	printf("head: %p, head->next: %p, cur: %p, cur->next: %p, swapped: %p\n", head, head->next, cur, cur->next, swapped);
					}
				}
			}
			else
			{
				if(prev > cur)
				{
					temp = cur;
					swapped->next = cur;
					cur->next = prev;
					prev->next = cur->next;
				}
			}
			prev = cur;
			cur = cur->next;
		//	printf("entering here - cur: %p\n", cur);
		}
		cur = head;
	}
		
#ifdef DEBUG
	printf("Sorting Done- HEAD: %p\n", head);
#endif
}

int list_count()
{
	node_t* cur_node = head;
	int n = 0;
	
	while(cur_node!= NULL)
	{
		n++;
		cur_node = cur_node->next;
	}
	return n;
}

void list_dump()
{
	node_t* cur_node;
	cur_node = head;
	int n = 0;

	printf("\n------------------------------MEM-DUMP--------------------------------\n");
	if(head == NULL)
	{
		printf("Node: %d, Start Address: %p , Available size: %d\n", n, head, 0);
	}

	while(cur_node != NULL)
 	{
		printf("Node: %d, Start Address: %p , Available size: %d\n", n, cur_node, cur_node->size);
		cur_node = cur_node->next;
		n++;
	}
}	
