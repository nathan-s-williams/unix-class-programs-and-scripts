//This is my SISH shell Program:
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>



int main(int argc, char *argv[]) {
	char *cmd;
	size_t sizeOfString;
	char *hist[99];
	int size = 0;
	//size_t *test = 0; 
	while(1) {
		//Display the prompt
		printf("sish> ");
		
		//Read command
		cmd = NULL;
		
		sizeOfString = 0;
		if(getline(&cmd, &sizeOfString, stdin) == -1) {
			free(cmd);
			perror("getline failed");
			exit(EXIT_FAILURE);
		}

		//Parsing and tokenizing the command
		char *token = strtok(cmd, " ");
		char *commandArg[10];
		int numArg = 0;
		while(token != NULL) {
			//printf(" %s \n", token);
			commandArg[numArg] = token;
			token = strtok(NULL, " ");
			numArg++;
		}
		char *argument = commandArg[numArg - 1];
		argument[strlen(argument) - 1] = '\0';
		
		//To see if it will put into an array properly
		int i = 0;
		for(i; i < numArg; i++) {
			printf("Argument #%d: %s \n", i, commandArg[i]); 
		} 

		hist[size] = (char*)malloc(strlen(commandArg[0]) * sizeof(char)); 		
		size++;
		//MAKING BUILT_IN COMMANDS
                 //EXIT COMMAND
		if(strcmp(commandArg[0], "exit") == 0) {
			 return(0);
		
		 //CD COMMAND
		} else if(strcmp(commandArg[0], "cd") == 0)  {
			if(numArg > 2) {
				printf("Please enter only 1 directory\n");
				continue;
			}
			
			else if(commandArg[1] == " " || commandArg[1] == NULL) {
				if(chdir(getenv("HOME")) != 0) {
					perror("cd has failed");
					continue;
				}			
			} else {
				if(chdir(commandArg[1]) != 0) {
					perror("cd has failed");
					continue;
				}
			}
		}
		else if(strcmp(commandArg[0], "history") == 0){
			printf("checkpoint 1 reached\n");
			if(commandArg[1] == NULL || strcmp(commandArg[1], "-c") == 0){
				printf("checkpoint 2 reached\n");
				int i;
				printf("checkpoint 3 reached\n");
				for(i = size; i >= 0 ; i--){
					printf("%s\n", hist[size]);
					//int x;
					//for(x = 0; x < strlen(hist[i]); x++){
					//	printf("%s
					//}
				}
				continue;
			}
			else if(strcmp(commandArg[1], "-c") == 0){
				printf("-c reached");
				continue;
			}
			else{
					printf("else reached");			
				}
			}
		}


		//Executing a command
		int cpid = fork();
		if(cpid == 0) {
			//printf("This is the child process\n");
			commandArg[numArg] = NULL;
			if(execvp(commandArg[0],commandArg) < 0 ) {
				printf("Command not found\n");
				exit(1);
			}
		} else {
			//printf("I am the parent\n");
			wait(NULL);
		}
		

	}
	free(cmd);
}
