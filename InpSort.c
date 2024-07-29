#include "tree.h"

#define MAXSTR 100
#define BLKSIZE 800000
#define ELSIZE 4
#define DIRE 0x1 << 31

list *Inp(char *name , list *root)
{
	FILE *inp ;
	uint32_t values[MAXLEN] = {0} ; //using malloc caused segmentation error
														//caused due to uaccounted characters giving very large indexes
	inp = fopen(name , "r") ;
	char c ;

	while((c = fgetc(inp)) != EOF)
	{
		values[c] += 1 ;
	}

	fclose(inp) ;

	HS *tmp ;

	for(int i = 0 ; i < MAXLEN ; i ++)
		if(values[i] != 0)
		{
			tmp = malloc(HSIZE) ;
			tmp -> val = i ;
			tmp -> freq = values[i] ;
			tmp -> rchild = tmp -> lchild = NULL ;
			root = addHS2List(tmp , root) ;
		}

	return root ; //The parameter can only change the value pointed by root not root itself
				  //therefore we have to return it or use pointer to pointer 
}

void CodeGen(HS *ptr , uint32_t arr[MAXLEN][2] , uint32_t code , uint32_t bits)
{

	if(ptr -> rchild != NULL)
	{
		CodeGen(ptr -> rchild , arr , ((code << 1) + 1) , bits + 1) ;
	}

	if(ptr -> lchild != NULL)
	{
		CodeGen(ptr -> lchild , arr , (code << 1) , bits + 1) ;
	}
	
	if(ptr -> val != NOCHAR)
	{
		arr[ptr -> val][0] = code ;
		arr[ptr -> val][1] = bits ;
		printf("Var %c Code %x Actual Bits : %zu \n" , ptr -> val , code , bits) ;
		return ;
	}


	return ;

}

uint32_t Encode(uint32_t code[MAXLEN][2] , char *name)
{
	FILE *inp = fopen(name , "r") ;

	uint8_t c = 0 ;

	uint32_t *ptr = malloc(4 * BLKSIZE) ;
	uint32_t top = 8*ELSIZE ;
	uint32_t tcode , bits ;
	size_t index = 0 ;

	uint32_t TotalBits = 0 ;
	while(((c = fgetc(inp)) != EOF) && c != '\t')
	{
		tcode = code[c][0] ;
		bits = code[c][1] ;

		printf("%zu" , tcode);

		if((bits > top) && top)
		{
			uint8_t tmp = bits - top ;
			ptr[index] = ptr[index] | (tcode >> tmp) ;
			index++ ;
			top = 8*ELSIZE ;
			tcode = tcode << (top - tmp) ;
			ptr[index] = ptr[index] | tcode ;
			top = top - tmp ;
			TotalBits += bits ;
			continue ;

		}
		else if(bits > top)
		{
			index ++ ;
			top = 8*ELSIZE ;
		}

		top = top - bits ;
		ptr[index] = ptr[index] | (tcode << (top)) ;
		TotalBits += bits ;

	}

	FILE *out = fopen("out" , "wb") ;
	fwrite(ptr , ELSIZE , index + 1 , out) ;

	free(ptr) ;
	fclose(inp) ;
	fclose(out) ;
	return TotalBits ;
}

void printBinary(char *name , uint32_t TotalBits)
{
	FILE *inp = fopen(name , "rb") ;

	uint32_t ptr[BLKSIZE] = {0} ;

	fread(ptr , ELSIZE , (TotalBits/32) + 1 , inp) ;

	for(int i = 0 ; i < (TotalBits/32) + 1 ; i++)
		printf("%x" , ptr[i]) ;

	printf("\n") ;
	return ;
}

void Decode(HS *root , char *name , uint32_t TotalBits)
{
	FILE *inp = fopen(name , "rb") ;

	size_t c = 0 ;

	HS *tmp = root ;
	uint32_t *ptr = malloc(4 * BLKSIZE) ;
	uint32_t index = 0 ;
	c = fread(ptr , ELSIZE , (TotalBits/32) + 1 , inp) ;
	uint32_t top = 0x1 << 31 ;

	while(TotalBits--)
	{
		if(tmp -> val != NOCHAR)
		{
			printf("%c" , tmp -> val) ;
			tmp = root ;
		}

		if(top == 0)
		{
			index++ ;
			top = 0x1 << 31 ;
		}

		if(ptr[index] & top)
		{
			tmp = tmp -> rchild ;
		}
		else
		{
			tmp = tmp -> lchild ;
		}

		top = top >> 1 ;
	}
	printf("\n") ;

	free(ptr) ;
	fclose(inp) ;
}

void SpitKey(uint32_t ptr[][2])
{
	FILE *out = fopen("key" , "wb") ;
	fwrite(ptr , 4 , MAXLEN *2 , out) ;
	fclose(out) ;
	return ;
}

int main(int argc , void *argv[argc - 1])
{
	uint8_t Encrypt = 0 , Unlock = 0 , spitKey = 0 ;
	char *name = malloc(sizeof(char) * MAXSTR) ;
	char *Key = NULL ;

	printf("ARGC : %zu \n" , argc) ;
	for(int i = 1 ; i < argc ; i ++)
	{
		switch(((char *)(argv[i]))[1])
		{
			case 'i' :
				if(i < (argc - 1))
				strcpy(name , argv[++i]) ;
				break ;

			case 'k' :
				spitKey = 1 ;
					printf("encryption;");
				goto NAME ;
				break ;

			case 'e':
				if(i < (argc - 1))
				{
					Key = malloc(sizeof(char) * MAXSTR) ;
					strcpy(Key , argv[++i]) ;
NAME:

					strcpy(name , argv[++i]) ;
				}
				else
					break ;
				Encrypt = 1 ;
				break ;

		}
	}
	if(!Encrypt)
	{
		list *root = NULL;
		root = Inp(name , root) ;
		printList(root) ;
		int z = NumElements ;
		uint32_t TotalBits ;
		for(int i = 1 ; i <= z -1  ; i++)
			root = joinFirstTwo(root) ;

		uint32_t arr[MAXLEN][2] = {0} ;
		CodeGen(root -> val , arr , 0 , 0) ;
		if(spitKey)
			SpitKey(arr) ;
		TotalBits = Encode(arr , "origtest") ;
		Decode(root -> val , "out" , TotalBits) ;
		free(root) ;
	}
	else
	{
		printf("In f") ;
		FILE *k = fopen(Key , "rb") ;
		uint32_t arr[MAXLEN][2] ;
		fread(arr , 4 , MAXLEN * 2 , k) ;
		Encode(arr , name) ;
	}

	exit(0) ;

}


