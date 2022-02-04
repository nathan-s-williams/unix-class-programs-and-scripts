
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern int tokenize(char *cmd, char *commandArg[]);
extern void excBuiltIn(int func, char *commandArg[], int numArg);
extern void excFile(char *commandArg[], int arrLength);

int main(int argc, const char * argv[]) {
    //Declare variables
    char *cmd;
    size_t sizeofString;
    char *hist[99];
    int size = 0;
    char *commandArg[10];
    while (1) {
        //Display the prompt
        printf("sish> ");
        
        //Read command
        cmd = NULL;
        sizeofString = 0;
        getline(&cmd, &sizeofString ,stdin);
        
        //Tokenize input and return number of args passed.
        int numArg = tokenize(cmd, commandArg);
        
        hist[size] = (char*) malloc(strlen(commandArg[0]) * sizeof(char));
        char *temp = commandArg[0];
        int i = 0;
        while(*temp != '\0'){
            *(hist[size] + i++) = *temp++;
        }
        size++;
        
        //Execute builtin
        if(strcmp(commandArg[0], "exit") == 0) {
            excBuiltIn(1, commandArg, numArg);
        }
        else if (strcmp(commandArg[0], "cd") == 0){
            if(numArg > 2){
                printf("Please only enter one directory.\n");
                continue;
            }
            excBuiltIn(2, commandArg, numArg);
        }
        else if (strcmp(commandArg[0], "history") == 0){
            excBuiltIn(3, commandArg, numArg);
        }
        else{
            excFile(commandArg, numArg);
        }
    }
    
    return 0;
}
