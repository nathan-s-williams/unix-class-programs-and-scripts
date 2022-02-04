#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXLINE 4096
void
err_exit(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}
int
main(int argc, char *argv[])
{
    int pipefd[2];
    int pipefd2[2];
    pid_t cpid1, cpid2;
    pid_t cpid3;
    if (argc != 2) {
     fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
     exit(EXIT_FAILURE);
    }

    //Create first pipe
    if (pipe(pipefd) == -1) 
        err_exit("pipe");
    //Create second pipe
    if (pipe(pipefd2) == -1) 
        err_exit("pipe");
    //fork first child
    cpid1 = fork();
    if (cpid1 == -1) 
        err_exit("fork");
	
	if (cpid1 == 0) {    /* child 1 */
        close(pipefd[0]); /* close all unused pipe ends  */
        close(pipefd2[0]);
        close(pipefd2[1]);
	if (dup2(pipefd[1], STDOUT_FILENO) != STDOUT_FILENO) //dup stdout to pipe out
             err_exit("dup2 error to stdin");
         close(pipefd[1]);   /* don't need this after dup2 */
	/* call exec with appropriate arguments*/
        char *myargs[] = {"cat", argv[1], 0};
        execvp(myargs[0], myargs);
		err_exit("execvp failed");
	}
    
    //fork second child
    cpid2 = fork();
    if (cpid2 == -1) 
        err_exit("fork2");
	
	if (cpid2 == 0) {    /* child 2 */
	close(pipefd[1]);   /* close all unused pipe ends  */
        close(pipefd2[0]);
	if (dup2(pipefd[0], STDIN_FILENO) != STDIN_FILENO) //dup stdin to pipe in
        	err_exit("dup2 error to stdin");
        close(pipefd[0]);   /* don't need this after dup2 */
	if (dup2(pipefd2[1], STDOUT_FILENO) != STDOUT_FILENO){ //dup stdout to pipe2 out
		err_exit("dup2 error to stdout");
	}
	close(pipefd2[1]); //close pipe2 out
	/* call exec with appropriate arguments*/
        char *myargs[] = {"cat", 0};
        execvp(myargs[0], myargs);
			err_exit("execvp failed");
	}

    //fork third child
    cpid3 = fork();
    if (cpid3 == -1) 
        err_exit("fork3");

	if (cpid3 == 0) {    /* child 2 */
	close(pipefd[0]);   /* close all unused pipe ends */
        close(pipefd[1]);
        close(pipefd2[1]);	
	if (dup2(pipefd2[0], STDIN_FILENO) != STDIN_FILENO) //dup stdin to pipe2 in
        	err_exit("dup2 error to stdin");
        close(pipefd2[0]);   /* don't need this after dup2 */
	/* call exec with appropriate arguments*/
        char *myargs[] = {"wc", 0};
        execvp(myargs[0], myargs);
			err_exit("execvp failed");
	}



	/* parent should close both ends of the pipe */
	close(pipefd[0]); /* close read end */
	close(pipefd[1]);   /* close write end */

	close(pipefd2[0]); /* close read end */
	close(pipefd2[1]);   /* close write end */
	/* wait for both the children */
	 if (waitpid(cpid1, NULL, 0) < 0)
            err_exit("waitpid error");
	 if (waitpid(cpid2, NULL, 0) < 0)
            err_exit("waitpid error");
        exit(EXIT_SUCCESS);
}

