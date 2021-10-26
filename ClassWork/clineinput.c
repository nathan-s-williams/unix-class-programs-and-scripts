//
//  clineinput.c
//  ClassWork
//
//  Created by Nathan Williams on 10/26/21.
//

#include <stdio.h>
#include <string.h>

//Splits line into individual tokens based on deliminator
//Returns number of args.
int tokenize(char *cmd, char *commandArg[]){
    int numArg = 0;
    
    //Parsing and tokenizing the command
    char *token = strtok(cmd, " ");
    while(token != NULL) {
        commandArg[numArg] = token;
        token = strtok(NULL, " ");
        numArg++;
    }
    
    //Replace newline with null value.
    char *argument = commandArg[numArg - 1];
    argument[strlen(argument) - 1] = '\0';
    
    return numArg;
}
