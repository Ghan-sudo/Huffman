#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#undef NULL
#define NULL (void*)(0) //A better approach to pointer null
#define NOCHAR (char)0 //to diffrentiate node from leaf
#define MAXLEN 256

static int NumElements = 0 ;
typedef struct HS HS  ;

struct HS
{
	HS *rchild ;
	HS *lchild ;

	char val ; 

	uint64_t freq ;
} ;

#define HSIZE sizeof(HS) //reduce computation

typedef struct list list ;
struct list
{
	list *prev ;
	list *nxt ;

	HS *val ;
} ;

#define LSIZE sizeof(list) 

void printList(list *root)
{
	list *ptr = root ;

	while(ptr != NULL)
	{
		printf("Variable : %c frequency : %zu \n" , ptr -> val -> val , ptr -> val -> freq) ;
		ptr = ptr -> nxt ;
	}
}

void printRevList(list *root)
{
	list *ptr = root ;
	while(ptr -> nxt != NULL)
		ptr = ptr -> nxt ;

	while(ptr != NULL)
	{
		printf("Variable : %c frequency : %d \n" , ptr -> val -> val , ptr -> val -> freq) ;
		ptr = ptr -> prev ;
	}
}

void printTree(HS *ptr)
{
	if(ptr -> val != NOCHAR)
	{
		printf("Var : %c Val : %zu \n" , ptr -> val , ptr -> freq) ;
		return ;
	}

	if(ptr -> rchild != NULL)
	{
		printTree(ptr -> rchild) ;
	}
	if(ptr -> rchild != NULL)
	{
		printTree(ptr -> lchild) ;
	}

	return ;
}

list *addHS2List(HS *a , list *root)
{
	NumElements++ ;
	if(root == NULL)
	{
		root = malloc(LSIZE) ;
		root -> prev = root -> nxt = NULL ;

		root -> val = a ;

		free(a) ;
		return root ;
	}

	list *tmp = root ;

	while(1)
	{
		if(a -> freq <= tmp -> val -> freq)
		{
			if(tmp -> prev != NULL)
			{
				list *prev = tmp -> prev ;
				prev -> nxt = malloc(LSIZE) ;
				prev -> nxt -> val = a ;
				prev -> nxt -> prev = prev ;
				prev -> nxt -> nxt = tmp ;
				tmp -> prev = prev -> nxt ;
			}

			else
			{
				list *prev = malloc(LSIZE) ;
				prev -> nxt = tmp ;
				prev -> prev = NULL ;
				tmp -> prev = prev ;
				prev -> val = a ;
				root = prev ;
			}

			return root ; 
		}

		if(tmp -> nxt == NULL)
			break ;

		tmp = tmp -> nxt ;
	}

	tmp -> nxt = malloc(LSIZE) ;
	tmp -> nxt -> val = a ;
	tmp -> nxt -> prev = tmp ;
	tmp -> nxt -> nxt = NULL ;

	return root ;
}

int indexHS(list *root , HS *ptr)
{
	int index = 0 ;

	list *tmp = root ;

	while(tmp != NULL)
	{
		index++ ;
		if(tmp -> val -> val == ptr -> val && tmp -> val -> freq == ptr -> freq)
		{
			return index ;
		}


		tmp = tmp -> nxt ;
	}
}

list *removeElement(list *root , int index)
{
	NumElements-- ;
	list *tmp = root ;
	int i = index - 1 ;
	while(i--)
	{
		tmp = tmp -> nxt ;
	}

	if(index > 1)
	{
		if(tmp -> nxt != NULL)
		{
			list *prev = tmp -> prev ;
			list *nxt = tmp -> nxt ;

			prev -> nxt = nxt ;
			nxt -> prev = prev ;
		}

		else
			tmp -> prev -> nxt = NULL ;

	}

	else
	{
		tmp -> nxt -> prev = NULL ;
		root = tmp -> nxt ;
	}

	return root ;

}

list *joinFirstTwo(list *root)
{
	HS *head = malloc(HSIZE) ;

	head -> val = NOCHAR ;
	head -> freq = root -> val -> freq + root -> nxt -> val -> freq ;
	if(root -> val -> freq > root -> nxt -> val -> freq)
	{
		head -> rchild = root -> val ;
		head -> lchild = root -> nxt -> val ;
	}

	else
	{
		head -> rchild = root -> nxt -> val ;
		head -> lchild = root -> val ;
	}

	root = addHS2List(head , root) ;

	root = removeElement(root , 1) ;
	root = removeElement(root , 1) ;

	return root ;
}
