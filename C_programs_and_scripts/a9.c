#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * This program counts the number of tokens in a given string.
 * Terminates when user enter BYE
 */

int
main(int argc, char *argv[])
{
	int num_tokens = 0;

	// define the required variables
	char *input = NULL;	//string variables
	char *token = NULL;
	char *delim = " \t";	//deliminators for space and tab
	int cmp = 0;
	size_t len = 0;
	ssize_t read;
	while (1) {
		printf("Enter the string: ");

		// use getline() to get the input string
		read = getline(&input, &len, stdin);
		if(read == -1){			//if read returns -1 print error and exit
			printf("Error: getline() failed.");
			EXIT_FAILURE;
		}

		// The input tokens are delimited by " ", "\t"
		// get the first token. use strtok or strtok_r
		// if it is "BYE" break from the loop (check strcmp/strncmp)
		token = strtok(input, delim);			//get first token
		if((token[strlen(token) - 1]) == '\n'){		//if last char in token is \n change to \0
			token[strlen(token) - 1] = '\0';
		}
		if((cmp = strcmp(token, "BYE")) == 0){		//check if token == BYE. If so break.
			break;			
		}
		
		// if  not, count the number of tokens
		// num_tokens should contain the number of tokens in the given string
		while(token != NULL) {				//loop through str until token is null
		        num_tokens++;				//count tokens
		        token = strtok(NULL, delim);
		}

		printf("number of tokens: %d \n", num_tokens);
		num_tokens = 0;

	}

	
	// free the memory allocated by getline
	free(input);
    exit(EXIT_SUCCESS);
}

