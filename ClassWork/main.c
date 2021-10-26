//
//  main.c
//  ClassWork
//
//  Created by Nathan Williams on 10/26/21.
//

#include <stdio.h>
#include <string.h>

int tokenize(char *cmd, char *commandArg[]);
void excBuiltIn(int func);
void excFile(char *commandArg[], int arrLength);

int main(int argc, const char * argv[]) {
    //Declare variables
    char *commandArg[10];
    
    //Display the prompt
    printf("sish> ");
    
    //Read command
    char *cmd = NULL;
    size_t sizeofString = 0;
    getline(&cmd, &sizeofString ,stdin);
    
    //Tokenize input and return number of args passed.
    int numArg = tokenize(cmd, commandArg);
    
    //Execute builtin
    if(strcmp(commandArg[0], "exit") == 0) {
        excBuiltIn(1);
    }
    else if (strcmp(commandArg[0], "cd")){
        excBuiltIn(2);
    }
    else if (strcmp(commandArg[0], "history")){
        excBuiltIn(3);
    }
    else{
        excFile(commandArg, numArg);
    }
    
    return 0;
}
