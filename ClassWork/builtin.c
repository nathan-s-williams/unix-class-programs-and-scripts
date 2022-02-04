
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void excBuiltIn(int func, char *commandArg[], int numArg){
    switch (func) {
        case 1:
            printf("Exit successful.");
            exit(0);
            break;
        case 2:
            if( numArg == 1 || strcmp(commandArg[1], "~\0") == 0 || strcmp(commandArg[1], "\0") == 0) {
                if(chdir(getenv("HOME")) != 0) {
                        perror("cd has failed");
                }
            }
            else {
                if(chdir(commandArg[1]) != 0) {
                        perror("cd");
                }
            }
            break;
        case 3:
            
            break;
            
    }
}
