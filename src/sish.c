/*********************************************************************************
 *PROJECT 1: CREATE A SHELL
 *Created by Nathan Williams and Akito Minosoko
 *Implements a command line interpreter or shell. Operates with limited
 *functionality such as:
 *1) Run system commands.
 *2) Run exit, cd and history built-ins.
 *3) Execute a chain of system commands using pipes.
*********************************************************************************/
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> //wait and waitpid
#include <sys/wait.h> // wait and waitpid
#include <ctype.h>

//Global variables for command arg length and length of each command.
//Also contains max number of pipe args to handle.
#define CMD_ARR_LENGTH 20
#define NUMPIPEARG  20


/*********************************************************************************
 *INPUT
 * This function will tokenize the input by the user in either one of two ways
 * If the variable 'pipeflag' is set to 1, the function will tokenize based on
 * the symbol '|'. If the variable 'pipeflag' is set to 0, the function will
 * tokenize based on ' '. All tokenized arguments will be stored in the array
 * 'commandArg'
*********************************************************************************/

void tokenize(char *cmd, char *commandArg[], int strArg[],int pipeflag){
	
	strArg[0] = 0; //number of args
    	strArg[1] = 0; //number of pipes
	
	if(pipeflag == 1){


    		//PARSE THE USER INPUT BY THE DELIMITER OF '|'
    		char *token = strtok(cmd, "|");
		while(token != NULL) {
			//Checks if the number of argument is greater than 20
			if(strArg[0] == 20) {
                        	printf("Warning: The maximum number of arguments is 20: It will run the first 20 arguments\n");
	                	return;
                	}
			commandArg[strArg[0]] = token;
			token = strtok(NULL, "|");
			strArg[1]++; //number of pipes
			strArg[0]++; //number of args
		}
		strArg[1]--; //number of pipes
	}
	else{	
		char *token = strtok(cmd, " ");
		while(token != NULL) {
			//Checks if the number of argument is greater than 20
			if(strArg[0] == 20) {
                        	printf("Warning: The maximum number of arguments is 20: It will run the first 20 arguments\n");
	               		return;
                	}
			commandArg[strArg[0]] = token;
        		token = strtok(NULL, " ");
        		strArg[0]++; //number of args
    		}
	
	}

}


/*******************************************************************************
 *HISTORY
 * The 'addToHistory' function will step through the 'commandArg' array
 * and add the user input into the 'hist' array. If the pipeflag set to 1
 * the function will place a '|' in-between each element of the 'commandArg'
 * array. If the pipeflag is set to 1, at the end of each element in 'hist',
 * the '~' symbol will be appeneded to mark the end position in the element 
*********************************************************************************/
void addToHistory(char * hist, char *commandArg[], int strArg, int pipeflag){
	int cmdnum;
	int strpos = 0;
	for(cmdnum = 0; cmdnum < strArg; cmdnum++){
		char *temp = commandArg[cmdnum];
		while(*temp != '\0'){
			*(hist + strpos++) = *temp++;
		}
		if((cmdnum + 1) != strArg){	
			if(pipeflag == 1){	
				*(hist + strpos++) = '|';
			}
			else {
				*(hist + strpos++) = ' ';
			}
		}
	}

	*(hist + strpos) = '~';
}



/*******************************************************************************
 *EXECUTE COMMAND
 * The 'execCommand' function will  fork a new process. While the
 * parent process waits for the child process to complete, the child
 * process will call execvp() and execute the 'commandArg' array
*********************************************************************************/

void excCommand(char *commandArg[], int strArg){
	int cpid = fork();
	if(cpid == -1) {
		perror("fork");
		return;
	}
	if(cpid == 0) {
		commandArg[strArg] = NULL;
		if(execvp(commandArg[0],commandArg) < 0 ) {
			printf("Command not found\n");
			exit(1);
	
		}
	
	} 
	else {
		wait(NULL);
	
	}
		
}



/*******************************************************************************
 *PIPE 
 *strArg[0] = number of args and [1] = pipes
 *
 * The 'pipeFunction' is called whenever there needs to be piped
 * arugment executed. The function starts off by first initializing
 * a 2D array (the right component being the number of pipes and
 * the left component either being the write end or the left end).
 * The function will pipe() the file descriptors of the initialized
 * 'pipefd'. Next the function fork the beginning process (the first
 * command in the pipe). Then the child process will close all
 * necessary pipes and redirect the necessary file descriptor to standard
 * output. The child process will then tokenzie the first argument to 'commandArg'
 * by ' ' and execute the command. If there are more than one pipes, the
 * middle child is forked and will wait until the first child process is
 * finished. Then the middle child will iterate the next few processes until
 * it reaches the last piped command. The middle child will close every
 * file descriptor execept for the read-end of the previous file descriptor
 * and the write-end of the next file descriptor. Then the middle child will
 * execvp() the corresponding command, relative to which argument it is
 * currently iterating. The last fork will, close all pipes execpt for the
 * read-end of the first. Then the process will execute the very last piped
 * arugment in the 'commandArg' array using execvp(). Meanwhile, the parent
 * process will close all file descriptors and wait for the child processes to finish.
*********************************************************************************/
void pipeFunction(char *commandArg[], int strArg[]) {
	int pipefd[strArg[1]][2], i;
	pid_t cpidStart, cpidMid, cpidEnd;
	
	for(i = 0; i < strArg[1]; i++) {
		if(pipe(pipefd[i]) == -1) {
			perror("pipe failed");
			return;
		}
	}


	
	//FIRST CHILD
	int p;
	cpidStart = fork();
	if(cpidStart == -1) {
		perror("Beginning Fork");
		return;
	}
	if(cpidStart == 0) {
		//close read end of first pipe
		close(pipefd[0][0]);	

		//close all other pipes 
		if(strArg[1] > 1) {
			for(i = 1; i < strArg[1]; i++) {
				for(p = 0; p < 2; p++) {
					close(pipefd[i][p]);
				}
			}
		}

		if(dup2(pipefd[0][1], STDOUT_FILENO) != STDOUT_FILENO) {
			perror("dup2 to stdout");
			return;
		}	
		close(pipefd[0][1]);
	
		//Tokenize specified first piped argument with " "		
		int numofPipeArgs = 0;	
		char *execarr[NUMPIPEARG];
		char *tokenpipe = strtok(commandArg[0], " ");
		while(tokenpipe != NULL) {
			execarr[numofPipeArgs] = tokenpipe;
			tokenpipe = strtok(NULL, " ");
			numofPipeArgs++;
		}
		execarr[numofPipeArgs] = NULL;
		
		//Execute first piped argument
		if(execvp(execarr[0], execarr) == -1) {
			perror("execvp");
			exit(1);
		}
	}

	//MIDDLE CHILD
	if(strArg[1] > 1) {
		if(waitpid(cpidStart, NULL, 0) < 0) {
			perror("waitpid for first fork");
			return;
		}	
	
	
	
		int currPipe = 0;
		int currArg = 1;
		int n;
		int m;
		for(i = 0; i < strArg[1] - 1; i++) {
			cpidMid = fork();
			if(cpidMid == -1) {
				perror("Middle fork");
				return;
			}
			if(cpidMid == 0) {
				for(m = 0; m < strArg[1]; m++) {
					for(n = 0; n < 2; n++) {
						if( (m == currPipe && n == 0) || (m == (currPipe + 1) && n == 1) ) {
							continue;
						} else {
							close(pipefd[m][n]);
						}
					}	
				}
		
				if(dup2(pipefd[currPipe][0], STDIN_FILENO) != STDIN_FILENO) {
					perror("dup2 error to stdin in middle fork");
					return;
				}
				close(pipefd[currPipe][0]);
			
			
				if(dup2(pipefd[currPipe + 1][1], STDOUT_FILENO) != STDOUT_FILENO) {
					perror("dup2 error to stdout in middle fork");
					return;
				}
				close(pipefd[currPipe + 1][1]);


				int numofPipeArgs2 = 0;	
				char *execarr2[NUMPIPEARG];
				char *tokenpipe2 = strtok(commandArg[currArg], " ");
				while(tokenpipe2 != NULL) {
					execarr2[numofPipeArgs2] = tokenpipe2;
					tokenpipe2 = strtok(NULL, " ");
					numofPipeArgs2++;
				}
				execarr2[numofPipeArgs2] = NULL;
			
				//Execute first piped argument
				if(execvp(execarr2[0], execarr2) == -1) {
					perror("execvp");
					exit(1);
		
				}
			}
			currPipe++;
			currArg++;
		}
	}





	//LAST CHILD
	cpidEnd = fork();
	if(cpidEnd == -1) {
		perror("Ending fork");
		return;
	}
	if(cpidEnd == 0) {
		int e;
		int f;
		//Close all other pipes
		if(strArg[1] > 1) {
			for(e = 0; e < strArg[1] - 1; e++) { 
				for(f = 0; f < 2; f++) {
					close(pipefd[e][f]);
				}
			}
		} 
		close(pipefd[strArg[1] - 1][1]);
		if(dup2(pipefd[strArg[1] - 1][0], STDIN_FILENO) != STDIN_FILENO) {
			perror("dup2 to stdin");
			return;
		}

		close(pipefd[strArg[1] - 1][0]);
		int numofPipeArgs3 = 0;
		char *execarr3[NUMPIPEARG];
		char *tokenpipe3 = strtok(commandArg[strArg[0] - 1], " ");
		while(tokenpipe3 != NULL) {
			execarr3[numofPipeArgs3] = tokenpipe3;
			tokenpipe3 = strtok(NULL, " ");
			numofPipeArgs3++;
		}
		execarr3[numofPipeArgs3] = NULL;
		if(execvp(execarr3[0], execarr3) == -1) {
			perror("execvp");
			exit(1);
		}
	}
		
	//PARENT OF ALL CHILD PROCESS
	int x;
	int y;
	for(x = 0; x < strArg[1]; x++) {
		for(y = 0; y < 2; y++) {
			close(pipefd[x][y]);
		}
	}
	
	
	int werr;	
	while((werr = wait(NULL)) > 0){
		if(werr == -1){
			perror("Parent wait error");
			return;
		}
	}
}


/*******************************************************************************
 *BUILT INS
 *
 * changedirectory:
 * The 'changedirectory' function will first ask the user to try again if
 * the user inputs more than 2 arguments (including the first argument).
 * If the user inputs 'cd', 'cd ~', or 'cd ', the function change the 
 * directory to it's "home" directory. If neither of the conditions fit,
 * the function will call the chdir() function to change to the certain
 * directory the user provided in their second arugment
 *
 * history:
 * The 'history' function is called when the first argument provided
 * by the user is 'history'. If the number of arguments is equal to 1,
 * then the function first print the number of the offset of the history 
 * array then it will go step through the 'hist' array and print the
 * arguments of the 'hist' array in respect to it's offset, each character
 * at time until it reaches the character '~' which was placed to signify
 * the end of the string. If the second argument of that the user provided 
 * was a '-c' then the function will free the malloced 'hist' array and ....
 * Finally, the 'history' function will check if hte second arugment
 * is a digit. If the second arugment is a digit, the atoi() function
 * is called to first convert the second argument provided by the user
 * to an integer variable type. Then the function will check if the offset
 * the '|' symbol and will set the pipeflag to 1 if '|' is contained within
 * the element of the offset of the 'hist' array
*********************************************************************************/

void changedirectory(char *commandArg[], int strArg){
	if(strArg > 2) {
		printf("Please enter only 1 directory\n");
			return;
        } 
	else if( strArg == 1 || strcmp(commandArg[1], "~\0") == 0 || strcmp(commandArg[1], "\0") == 0) {
		if(chdir(getenv("HOME")) != 0) {
			perror("cd has failed");
			return;
		}

	} 
	else {
		if (chdir(commandArg[1]) != 0) {
			perror("cd has failed");
			return;
		}
               	
	}
}

void history(char *hist[], char *cmd, char *commandArg[], int arrPointer, int strArg[]){
	if(strArg[0] == 1){
		int i;
		char *temp;
			for(i = 0; i < arrPointer; i++){ //Stop at one below current pointer
				temp = hist[i];
				printf("%d  ", i);
				while(*temp != '~'){
					printf("%c", *temp);
					temp++;
				}
				printf("\n");
			}
	}
	else if(strcmp(commandArg[1], "-c") == 0){
		int strlength;
		int i;
		for(i = 0; i < arrPointer; i++){
			strlength = strlen(hist[i]);
			memset(hist[i], 0, strlength);
		}
	}
	else{ 
		if(isdigit(*commandArg[1]) != 0){
			int offset = atoi(commandArg[1]);
			
			if(offset >= arrPointer || offset < 0 || offset > 99){
				printf("History offset out of bounds.\n");
				return;
			}

			if(arrPointer < 100)
				strcpy(cmd, hist[offset]); //Use strcpy for true copy.
			else
				strcpy(cmd, hist[offset - 1]); 
			cmd[strlen(cmd) - 1] = '\0';
			//CHECK IF THE USER INPUTS A PIPE
			int doesitPipe;
			if(strchr(cmd, '|') != NULL) {
				doesitPipe = 1;
			}
			
			tokenize(cmd, commandArg, strArg, doesitPipe);
            		if(doesitPipe == 1)
	    			pipeFunction(commandArg, strArg);	
	    		else
	    			excCommand(commandArg, strArg[0]);
		}
		else {	
			//Error: command not found
			printf("Command not found\n");
		}
	}

}

/*******************************************************************************
 *MAIN
 * In the main function, the function will step into an infinite
 * while loop. In the while loop, the function will first print
 * the prompt 'sish >' while the program waits for the user input
 * next the function will call a getline() function to get the input
 * provided by the user, into a pointer called 'cmd'. Next the
 * variable 'doesitpipe' is initialized (that acts as a flag if the
 * user input '|'). The variable strsize is initialized (which is
 * used to store the length of the input). Then we add the null
 * character at the end of 'cmd' to ensure that the arugment will
 * execute accordingly. The 'hist' array is then malloced (which
 * is used to store the input of the user. The function will then
 * call the 'tokenize' function to tokenize dependent on the pipeflag
 * then the 'addToHistory' function will be called to place the
 * user input into the 'hist' array. If the user input is just "exit"
 * then the function will 'break' out of the while loop and free
 * the 'cmd' variable malloced by the getline() function and will free
 * the 'hist' array. If the first argument of the user input is 'cd',
 * then the function will call the 'changedirectory' function. Lastly,
 * if the 'doesitPipe' variable is set to one, the function will call
 * the 'pipeFunction' function. If the 'doesitpipe' variable is set to 0
 * then the function will call the 'execCommand' function.
*********************************************************************************/

int main(int argc, const char * argv[]) {
    //Declare variables
    char *cmd;
    char *commandArg[CMD_ARR_LENGTH]; //max arguments is 20
    size_t sizeOfString;
    char *hist[100];
    int arrPointer = 0;
    int maxPointer = 0;
    int strArg[2]; //[0] = number of args and [1] = number of pipes
    int doesitPipe;
    int strsize = 0;
    while (1) {
        
        //Display the prompt
        printf("sish> ");
        
        //Set variables
        cmd = NULL;
	int i;
	for(i = 0; i < CMD_ARR_LENGTH; i++){
		commandArg[i] = NULL;
	}
	
	sizeOfString = 0;
	doesitPipe = 0;

	//Read command
	if(getline(&cmd, &sizeOfString, stdin) == -1) {
		free(cmd);
		perror("getline failed");
		exit(EXIT_FAILURE);
	}

	doesitPipe = 0;
	strsize = strlen(cmd);
	cmd[strsize - 1] = '\0';

	//IF THE USER JUST INPUTS 'ENTER'
	if(strsize == 1) {
		continue;
	}

	 //If the user inputs only '|' without an argument
        if(cmd[strsize - 2] == '|') {
                printf("Please enter another command or argument after pipe\n");
                continue;
        }

	//CHECK IF THE USER INPUTS A PIPE
	if(strchr(cmd, '|') != NULL) {
		doesitPipe = 1;
	}

	tokenize(cmd, commandArg, strArg, doesitPipe);

	if(arrPointer < 100){
		hist[arrPointer] = (char*)malloc(strsize * sizeof(char));
		addToHistory(hist[arrPointer++], commandArg, strArg[0], doesitPipe);
	}
	else{
		int i;
		for(i = 0; i < arrPointer - 1; i++){
			memset(hist[i], 0, strlen(hist[i]));
			strcpy(hist[i], hist[i + 1]);
		}
		hist[arrPointer - 1] = (char*)malloc(strsize * sizeof(char));	
		addToHistory(hist[arrPointer - 1], commandArg, strArg[0], doesitPipe);
	}
       
       //Execute builtin
        if(strcmp(commandArg[0], "exit") == 0) {
		memset(hist, 0, 0);
		break;
	
	}
        else if (strcmp(commandArg[0], "cd") == 0){
        	changedirectory(commandArg, strArg[0]);
	
	}
        else if (strcmp(commandArg[0], "history") == 0){
		history(hist, cmd, commandArg, arrPointer, strArg);
	
		if(commandArg[1] != NULL && strcmp(commandArg[1], "-c") == 0){
			if(arrPointer > maxPointer)
				maxPointer = arrPointer;
			arrPointer = 0;
		}
	}
        else{
            if(doesitPipe == 1)
	    	pipeFunction(commandArg, strArg);	
	    else
	    	excCommand(commandArg, strArg[0]);
        
	}
    
    }
    
    free(cmd);
    int i;
    for(i = 0; i < maxPointer; i++){
	free(hist[i]);	
    }
    
    return 0;

}
