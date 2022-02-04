#include <stdint.h>

void reverseArray(uint32_t src[], int32_t size); //Prototype

//Function that reverses an array in place without the use of an intermediary variable per requirements.
void reverseArray(uint32_t src[], int32_t size){
	int i;
        for(i = 0; i < size / 2; i++){				//Loop through half of the array as operations take place on
								//both ends of the array simultaneously.
        	src[i] = src[i] + src[size - 1 - i];		//Replace start with the addition of both ends
        	src[size - 1 - i] = src[i] - src[size - 1 - i];	//Replace end with the subtraction of end from i
        	src[i] = src[i] - src[size - 1 - i];		//Replace i with the subtraction of end from i
        }
}
