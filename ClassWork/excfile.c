
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void excFile(char *commandArg[], int arrLength){
    
    //Executing a command
    int cpid = fork();
    if(cpid == 0) {
        //printf("This is the child process\n");
        commandArg[arrLength] = NULL;
        if(execvp(commandArg[0],commandArg) < 0 ) {
            printf("Command not found");
            exit(1);
        }
    } else {
        //printf("I am the parent\n");
        wait(NULL);
    }
}
