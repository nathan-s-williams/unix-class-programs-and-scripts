//Assignment 6

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void reverseArray(uint32_t src[], int32_t size);

int main (int argc, char * argv){
	//char *temp = &argv[1];
	//int input = atoi(temp);

	//printf("%d", input);
	int32_t arr[] = {2, 5, 8};
	reverseArray(arr, 3);
}

void reverseArray(uint32_t src[], int32_t size){
	int i;
	//printf("Array before: ");
	//for(i = 0; i < size; i++){
	//	printf("%d ", src[i]);
	//}
	//i = 0;
	//printf("\n");
	//printf("%d", i);
	for(i = 0; i < size / 2; i++){
		src[i] = src[i] * src[size - 1 - i];
		src[size - 1 - i] = src[i] / src[size - 1 - i];
		src[i] = src[i] / src[size - 1 - i];
	}
	i = 0;
	
	printf("Array after: ");
	for(i = 0; i < size; i++){
		printf("%d ", src[i]);
	}
}


