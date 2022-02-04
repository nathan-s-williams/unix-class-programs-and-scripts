/*****************************************************************
 *Project2 server.c
 *****************************************************************/
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>

#define BUFMAX 1024

struct threadargs {
    int client_fd;
    struct sockaddr *addr;
    size_t addrlen;
};

void Usage(char *progname);
void PrintOut(int fd, struct sockaddr *addr, size_t addrlen, int threadid);
void PrintServerSide(int client_fd, int sock_family);
int  Listen(char *portnum, int *sock_family);
void *HandleClient(void* targs);
void tokenize(char *str, int *wnum, int *cnum);
void shortenarray(char dest[], char src[], int length);

int
main(int argc, char **argv) {
    // Expect the port number as a command line argument.
    if (argc != 2) {
        Usage(argv[0]);
    }
    
    int sock_family;
    int listen_fd = Listen(argv[1], &sock_family);
    if (listen_fd <= 0) {
        // We failed to bind/listen to a socket.  Quit with failure.
        printf("Couldn't bind to any addresses.\n");
        return EXIT_FAILURE;
    }
    
    pthread_t p1;
    struct threadargs targs;
    
    // Loop forever, accepting a connection from a client and doing
    // an echo trick to it.
    while (1) {
        struct sockaddr_storage caddr;
        socklen_t caddr_len = sizeof(caddr);
        int client_fd = accept(listen_fd,
                               (struct sockaddr *)(&caddr),
                               &caddr_len);
        if (client_fd < 0) {
            if ((errno == EINTR) || (errno == EAGAIN) || (errno == EWOULDBLOCK))
                continue;
            printf("Failure on accept:%d \n ", strerror(errno));
            break;
        }
        targs.client_fd = client_fd;
        targs.addr = (struct sockaddr *)(&caddr);
        targs.addrlen = caddr_len;
        pthread_create(&p1, NULL, HandleClient, &targs);
        
    }
    
    // Close socket
    close(listen_fd);
    return EXIT_SUCCESS;
}

void Usage(char *progname) {
    printf("usage: %s port \n", progname);
    exit(EXIT_FAILURE);
}

void
PrintOut(int fd, struct sockaddr *addr, size_t addrlen, int threadid) {
    
    if (addr->sa_family == AF_INET) {
        // Print out the IPV4 address and port
        char astring[INET_ADDRSTRLEN];
        struct sockaddr_in *in4 = (struct sockaddr_in *)(addr);
        inet_ntop(AF_INET, &(in4->sin_addr), astring, INET_ADDRSTRLEN);
        printf("worker %d: established connection with client %s", threadid, astring);
        printf("#%d\n", ntohs(in4->sin_port));
        
    } else if (addr->sa_family == AF_INET6) {
        // Print out the IPV6 address and port
        char astring[INET6_ADDRSTRLEN];
        struct sockaddr_in6 *in6 = (struct sockaddr_in6 *)(addr);
        inet_ntop(AF_INET6, &(in6->sin6_addr), astring, INET6_ADDRSTRLEN);
        printf("worker %d: established connection with client %s", threadid, astring);
        printf("#%d\n", ntohs(in6->sin6_port));
        
    } else {
        printf(" ???? address and port ???? \n");
    }
}

int
Listen(char *portnum, int *sock_family) {
    // Populate the "hints" addrinfo structure for getaddrinfo().
    // ("man addrinfo")
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       // IPv6 (also handles IPv4 clients)
    hints.ai_socktype = SOCK_STREAM;  // stream
    hints.ai_flags = AI_PASSIVE;      // use wildcard "in6addr_any" address
    hints.ai_flags |= AI_V4MAPPED;    // use v4-mapped v6 if no v6 found
    hints.ai_protocol = IPPROTO_TCP;  // tcp protocol
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    
    // Use argv[1] as the string representation of our portnumber to
    // pass in to getaddrinfo().  getaddrinfo() returns a list of
    // address structures via the output parameter "result".
    struct addrinfo *result;
    int res = getaddrinfo(NULL, portnum, &hints, &result);
    
    // Did addrinfo() fail?
    if (res != 0) {
        printf( "getaddrinfo failed: %s", gai_strerror(res));
        return -1;
    }
    
    // Loop through the returned address structures until we are able
    // to create a socket and bind to one.  The address structures are
    // linked in a list through the "ai_next" field of result.
    int listen_fd = -1;
    struct addrinfo *rp;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        listen_fd = socket(rp->ai_family,
                           rp->ai_socktype,
                           rp->ai_protocol);
        if (listen_fd == -1) {
            // Creating this socket failed.  So, loop to the next returned
            // result and try again.
            printf("socket() failed:%d \n ", strerror(errno));
            listen_fd = -1;
            continue;
        }
        
        // Configure the socket; we're setting a socket "option."  In
        // particular, we set "SO_REUSEADDR", which tells the TCP stack
        // so make the port we bind to available again as soon as we
        // exit, rather than waiting for a few tens of seconds to recycle it.
        int optval = 1;
        setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR,
                   &optval, sizeof(optval));
        
        // Try binding the socket to the address and port number returned
        // by getaddrinfo().
        if (bind(listen_fd, rp->ai_addr, rp->ai_addrlen) == 0) {
            // Bind worked!  Print out the information about what
            // we bound to.
            //PrintOut(listen_fd, rp->ai_addr, rp->ai_addrlen);
            
            // Return to the caller the address family.
            *sock_family = rp->ai_family;
            break;
        }
        
        // The bind failed.  Close the socket, then loop back around and
        // try the next address/port returned by getaddrinfo().
        close(listen_fd);
        listen_fd = -1;
    }
    
    // Free the structure returned by getaddrinfo().
    freeaddrinfo(result);
    
    // If we failed to bind, return failure.
    if (listen_fd == -1)
        return listen_fd;
    
    // Success. Tell the OS that we want this to be a listening socket.
    if (listen(listen_fd, SOMAXCONN) != 0) {
        printf("Failed to mark socket as listening:%d \n ", strerror(errno));
        close(listen_fd);
        return -1;
    }
    
    // Return to the client the listening file descriptor.
    return listen_fd;
}

void *
HandleClient(void* targs) {
    struct threadargs *args;
    args = (struct threadargs *)targs;
    int client_fd = args->client_fd;
    pid_t workerid = (pid_t) syscall (SYS_gettid);
    PrintOut(client_fd, args->addr, args->addrlen, workerid);
    // Print out information about the client.
    
    // Loop, reading data and echo'ing it back, until the client
    // closes the connection.
    while (1) {
        
        char clientbuf[BUFMAX];
        ssize_t res = read(client_fd, clientbuf, BUFMAX - 1);
        
        if (res == -1) {
            if ((errno == EAGAIN) || (errno == EINTR))
                continue;
            
            printf(" Error on client socket:%d \n ", strerror(errno));
            break;
        }
        clientbuf[res] = '\0';
        
        //Find the newline character and replace with '\0'
        int i;
        for(i = 0; i < BUFMAX; i++) {
            if(clientbuf[i] == '\n') {
                clientbuf[i] = '\0';
                break;
            }
        }
        int wnum;
        int cnum;
        char *str = (char*)malloc((BUFMAX - 1) * sizeof(char));
        
        strncpy(str, clientbuf, (BUFMAX - 1));
        tokenize(str, &wnum, &cnum);
        char cwrite[10];
        
        if(strcmp(clientbuf, "exit") == 0) {
            printf("worker %d: client terminated\n", workerid);
            break;
        }
        else{
            fprintf(stdout, "worker %d: received message from client. # of words = %d and # of characters = %d\n", workerid, wnum, cnum);
        }
        
        fflush(stdout);
        sprintf(cwrite, "%d %d\n", wnum, cnum);
        write(client_fd, cwrite, strlen(cwrite));
        free(str);
        str = NULL;
        
    }
    
    close(client_fd);
    return NULL;
    
}


void
tokenize(char *str, int *wnum, int *cnum){
    *wnum = 0;
    *cnum = 0;
    int numofSpaces = 0;
    int i;
    int length = strlen(str);
    for(i = 0; i < length; i++) {
        if(str[i] == ' ') {
            numofSpaces++;
        }
    }
    
    char *token = strtok(str, " ");
    while(token != NULL) {
        token = strtok(NULL, " ");
        (*wnum)++; //number of words
    }
    
    *cnum = length - numofSpaces;
    
}

