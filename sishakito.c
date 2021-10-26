//This is my SISH shell Program:
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

void executeFile();

int main(int argc, char *argv[]) {
	while(1) {
		//Display the prompt
		printf("sish> ");
		
		//Read command
		char *cmd = NULL;
		size_t sizeofString = 0;
		getline(&cmd, &sizeofString ,stdin);
		//printf("The command is: %s \n", command);

		//Parsing and tokenizing the command
		char *token = strtok(cmd, " ");
		char *commandArg[10];
		int numArg = 0;
		while(token != NULL) {
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


		//MAKING BUILT_IN COMMANDS
                 //EXIT COMMAND
		if(strcmp(commandArg[0], "exit") == 0) {
			 return(0);
		}
		



		//Executing a command
		int cpid = fork();
		if(cpid == 0) {
			//printf("This is the child process\n");
			commandArg[numArg] = NULL;
			if(execvp(commandArg[0],commandArg) < 0 ) {
				printf("Command not found");
				exit(1);
			}
		} else {
			//printf("I am the parent\n");
			wait(NULL);
		}
		

	}
}
