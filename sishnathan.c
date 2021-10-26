#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void builtin(char *command){
	if(command == "exit"){
		printf("Exiting");
		exit(0);
	}
}


int main(int argc, char *argv[]){
    while(1){
        char *str = NULL;
        char *myargs[10];
	size_t bufsize = 0;
	
	printf("sish> ");
        getline(&str, &bufsize, stdin);
	//printf("This is your command: %s", command);        
        char *token = strtok(str," ");
	int i = 0;
	while(token != NULL){
		myargs[i] = token;
		//printf("%s\n",token);
		size_t length = strlen(myargs[i]);
		char *str = myargs[i];
		if(str[length - 1] == '\n'){
			str[length - 1] = '\0';
			myargs[i] = str;
		}
			
		token = strtok(NULL," ");
		i++;
	}
	//myargs[i] = NULL;
	int x = i;
	printf("\n");
	for(i = 0; i < x; i++){
		printf("myargs[%d]: %s\n", i, myargs[i]);
	}


	if(myargs[0] == "exit"){
		builtin(myargs[0]);
	}
	else {
	int rc = fork();
	if(rc == 0){
		int return_status = execvp(myargs[0], myargs);
		printf("Error!");
		exit(1);
	}
	else {
		wait(NULL);
		printf("Now I'm the parent\n");
	}
 	}//end loop       
    }
}
