#include <stdio.h>     // for sscanf, fprintf, perror
#include <stdint.h>    // for int32_t
#include <inttypes.h>  // for SCNd32, PRId32
#include <assert.h>    // for assert
#include <string.h>    // for strlen
#include <inttypes.h>  // for SCNd32, PRId32
#include <sys/types.h> // for syscalls
#include <unistd.h>    // for lseek
#include <stdlib.h>    // for malloc, exit, EXIT_SUCCESS, EXIT_FAILURE
#include <errno.h>     // for EINTR
#include "sr.h"

/// include files are not complete. include the appropriate files


// Get all the fields of student record sr from stdin
// Put the record in the appropriate offset of the file described by fd
void
put(int32_t fd)
{
    sr s;
    while (1) {
        printf("Enter the student name: ");
        // WRITE THE CODE to read the name from stdin
        // store it in s.name
        // use fgets()
        // fgets doesnt remove newline. replace '\n' with '\0' in s.name. strlen() will be useful
        
        // Store name in s.name from stdin with a maximum of 30.
        // Use strlen to get str length and replace last character "\n" with "\0".
        // If no name was entered, reprompt the user, otherwise break loop.
        fgets(s.name, 30, stdin);
        size_t length = strlen(s.name);
        if(s.name[length - 1] == '\n')
            s.name[length - 1] = '\0';
        if (strlen(s.name) == 0) {
            printf("Please enter a valid name.\n");
            continue;
        }
        break;
    }
        
        
    printf("Enter the student id: ");
    // WRITE THE CODE to read student id from stdin
    // store it in s.id
    scanf("%" SCNd32 "%*c", &s.sid);

    printf("Enter the record index: ");
    //
    // WRITE THE CODE to read record index from stdin
    // store it in s.index
    scanf("%" SCNd32 "%*c", &s.index);
    
    // WRITE THE CODE to seek to the appropriate offset in fd (lseek(), sizeof() will be useful)
    // If lseek fails, output perror and exit with EXIT_FAILURE.
    long ls;
    if((ls = lseek(fd, s.index * sizeof(s), 0) == -1)){
        perror("Could not find index");
        exit(EXIT_FAILURE);
    }

    // WRITE THE CODE to write record s to fd
    // If write fails, output perror and exit with EXIT_FAILURE.
    size_t w;
    if((w = write(fd, &s, sizeof(s))) == -1){
        perror("Could not write data");
        exit(EXIT_FAILURE);
    }
    
}

// read the student record stored at position index in fd
void
get(int32_t fd)
{
    sr s;
    int32_t index;

    printf("Enter the record index: ");
    // WRITE THE CODE to get record index from stdin and store in it index
    scanf("%" SCNd32 "%*c", &index);
    // WRITE THE CODE to seek to the appropriate offset in fd
    // If lseek fails, output perror and exit with EXIT_FAILURE.
    long ls;
    if((ls = lseek(fd, index * sizeof(s), 0)) == -1){
        perror("Could not find index");
        exit(EXIT_FAILURE);
    }

    // WRITE THE CODE to read record s from fd
    // If read fails or reads an empty index, output error and exit with EXIT_FAILURE.
    size_t r;
    if((r = read(fd, &s, sizeof(s))) == -1){
        perror("Could not find index");
        exit(EXIT_FAILURE);
    }
    else if (r == 40 && strlen(s.name) == 0){   //Offset between 0 and highest index but no record exists. Read returns 40.
        printf("Error: No record returned.\n");
        exit(EXIT_FAILURE);
    }
    else if (r == 0){
        printf("Error: No record returned.\n"); //Offset outside of highest index. Read returns 0.
        exit(EXIT_FAILURE);
    }

    printf("Student name: %s \n", s.name);
    printf("Student id: %d \n", s.sid);
    printf("Record index: %d \n", s.index);

//    assert(index == s.index);   //Debug
}
