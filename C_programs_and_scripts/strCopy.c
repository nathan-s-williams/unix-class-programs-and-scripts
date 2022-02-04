#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define DEBUG 0

/* 
 * This function copies src string to *dst
 * memory for *dst should be allocated
*/
void strCopy(char* src, char** dst)
{
	// define required variables
	int size = 0;	  //store size of incoming string.
	int pOffset = 0;  //pointer offset variable that allows original pointer to keep pointing to same mem address.
	int i;		  	
	// Find the length of source (src) string
	// Use pointers and pointer arithmetic. 
	// Do not treat src.
	while(*(src + pOffset) != '\0'){  //move through string using pointer offset until null char is reached.
		size++;
		pOffset++;
	}

	// allocate memory for destination (*dst) string
	char *str = (char*) malloc(size * sizeof(char));
	// if memory allocation fails print appropriate error message using perror() 
	// and return
	if(str == NULL)
		perror("Error: Memory allocation failed.");

	
	// copy src to *dst. 
	// Use pointers and pointer arithmetic. 
	// Do not treat src and *dst as arrays.
	for(i = 0; i < size; i++){
		*(str + i) = *src++;	//copy chars in src string to memory in allocated space.
	}
	*dst = str;		 	//point dst to allocated memory containing the original string.

#if DEBUG
	printf("str: %s \n", str);
	printf("str: %s \n", *dst);
#endif
	
	return;
}

