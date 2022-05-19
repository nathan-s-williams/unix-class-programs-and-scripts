/*****************************************************************
 * Project2 - client.c
 * ===============================================================
 * Files: server.c client.c
 * Server:
 * Create a server.c program that handles multiple clients using
 * threads. Each thread must handle a single client and return 
 * the number of words and letters entered by the client. The
 * program should also notify when a new client joins and leaves
 * as well as the values returned to the client after they enter
 * a string. The max number of clients the program can accomodate
 * should be limited by the system.
 *
 * Client:
 * Should establish a connection with a server by passing
 * the server host name and port. Once a channel is opened with
 * the server, it should be able to enter any string and get
 * the number of words and letters returned to it.
 *****************************************************************/



#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUF 1024  //maximum size of the read and write buffer


//Protoype of all of the necessary functions
void Usage(char *progname);

int LookupName(char *name,
                unsigned short port,
                struct sockaddr_storage *ret_addr,
                size_t *ret_addrlen);

int Connect(const struct sockaddr_storage *addr,
             const size_t addrlen,
             int *ret_fd);




/*****************************************************
		MAIN FUNCTION 
*****************************************************/
/* In the main function, we first check if the user inputs
 * appropriate commands. If not, we call the 'Usage' 
 * function. Then we obtain the port number provided.
 * Next we invoke the 'LookUpName' function to get the
 * IP address of the hostname. Next we invoke the 'Connect'
 * function in order to connect the found IP address to
 * the file descriptor. Now, we enter into an infinite
 * while loop to invoke read() and write() to the server.
 * We first 'write' to the connected file descriptor.
 * If the client inputs "exit" we 'break' out of the
 * while loop and close the file descriptor. If the
 * client does not input exit, we will reset the 
 * buffer and use the same buffer to 'read' the information
 * sent from the server. We then print the buffer to
 * the standard output of the client. Finally, we reset
 * the buffer once again to 'write' the next piece of
 * information the client inputs.
 */


int 
main(int argc, char **argv) {
  if (argc != 3) {
    Usage(argv[0]);
  }

  unsigned short port = 0;
  if (sscanf(argv[2], "%hu", &port) != 1) {
    Usage(argv[0]);
  }

  // Get an appropriate sockaddr structure.
  struct sockaddr_storage addr;
  size_t addrlen;
  if (!LookupName(argv[1], port, &addr, &addrlen)) {
    exit(EXIT_FAILURE);
  }

  // Connect to the remote host.
  int socket_fd;
  if (!Connect(&addr, addrlen, &socket_fd)) {
    exit(EXIT_FAILURE);
  }

  // Read something from the remote host.
  // Will only read BUF-1 characters at most.
  char readbuf[BUF];
  int res;
 

  // Write something to the remote host.
  while (1) {
   int n = 0;
   while ((readbuf[n++] = getchar()) != '\n');
   int wres = write(socket_fd, readbuf, BUF-1);
    if (wres == 0) {
     printf("socket closed prematurely in write \n");
      close(socket_fd);
      return EXIT_FAILURE;
    }
    if (wres == -1) {
      if (errno == EINTR)
        continue;
      printf("socket write failure \n");
      close(socket_fd);
      return EXIT_FAILURE;
    }
    if(strncmp(readbuf, "exit", 4) == 0) {
      break;
    }
   // break;
   
   bzero(readbuf, sizeof(readbuf));
    res = read(socket_fd, readbuf, BUF-1);
    if (res == 0) {
      printf("socket closed prematurely \n");
      close(socket_fd);
      return EXIT_FAILURE;
    }
    if (res == -1) {
      if (errno == EINTR)
        continue;
      printf("socket read failure \n");
      close(socket_fd);
      return EXIT_FAILURE;
   }
   readbuf[res] = '\0';
   printf("%s", readbuf);
   bzero(readbuf, sizeof(readbuf));
}
  // Clean up.
  close(socket_fd);
  return EXIT_SUCCESS;
}



/*****************************************************
		USAGE FUNCTION 
*****************************************************/

/* This function will explicitly tell the user how to 
 * make use of the actual program and will be called in
 * the main function.
 */

void 
Usage(char *progname) {
  printf("usage: %s  hostname port \n", progname);
  exit(EXIT_FAILURE);
}








/*****************************************************
		LOOKUPNAME FUNCTION 
*****************************************************/

/* The 'LookUpName' function will first reset the 'hints'
 * struct then assign the approriate hints for the
 * 'getaddrinfo' function to find the IP address of the
 * provided hostname (server). We place the found address
 * into the 'results' struct. Next, we place the port
 * number provided into the approriate field of the actual
 * IP Address struct. Much like the previous step, we use
 * the 'result' struct to assign the IP address into the 
 * actual IP Address struct. Finally, we free the 'result'
 * struct.
 */

int 
LookupName(char *name,
                unsigned short port,
                struct sockaddr_storage *ret_addr,
                size_t *ret_addrlen) {
  struct addrinfo hints, *results;
  int retval;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  // Do the lookup by invoking getaddrinfo().
  if ((retval = getaddrinfo(name, NULL, &hints, &results)) != 0) {
    printf( "getaddrinfo failed: %s", gai_strerror(retval));
    return 0;
  }

  // Set the port in the first result.
  if (results->ai_family == AF_INET) {
    struct sockaddr_in *v4addr =
            (struct sockaddr_in *) (results->ai_addr);
    v4addr->sin_port = htons(port);
  } else if (results->ai_family == AF_INET6) {
    struct sockaddr_in6 *v6addr =
    	(struct sockaddr_in6 *)(results->ai_addr);
   v6addr->sin6_port = htons(port);
  } else {
    printf("getaddrinfo failed to provide an IPv4 or IPv6 address \n");
    freeaddrinfo(results);
    return 0;
  }

  // Return the first result.
  assert(results != NULL);
  memcpy(ret_addr, results->ai_addr, results->ai_addrlen);
  *ret_addrlen = results->ai_addrlen;

  // Clean up.
  freeaddrinfo(results);
  return 1;
}


/*****************************************************
		CONNECT FUNCTION 
*****************************************************/

/* Once we have the type of IP Address of the server, we use
 * this to create a socket (which will return a file
 * descriptor). Next, we connect the file descriptor to
 * the server IP Address. Next we print out the server
 * IP Address that we connected to. We now return the file
 * descriptor created by the 'socket' function.
 */

int 
Connect(const struct sockaddr_storage *addr,
             const size_t addrlen,
             int *ret_fd) {
  // Create the socket.
  int socket_fd = socket(addr->ss_family, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    printf("socket() failed: %s", strerror(errno));
    return 0;
  }

  // Connect the socket to the remote host.
  int res = connect(socket_fd,
                    (const struct sockaddr *)(addr),
                    addrlen);
  if (res == -1) {
    printf("connect() failed: %s", strerror(errno));
    return 0;
  }
  
 //Prints out the IP address of the server the client connected with
 printf("established connection with server: ");
  if (addr->ss_family == AF_INET) {
    // Print out the IPV4 address
    char astring[INET_ADDRSTRLEN];
    struct sockaddr_in *in4 = (struct sockaddr_in *)(addr);
    inet_ntop(AF_INET, &(in4->sin_addr), astring, INET_ADDRSTRLEN);
    printf("%s\n", astring);

  } else if (addr->ss_family == AF_INET6) {
    // Print out the IPV6 address
    char astring[INET6_ADDRSTRLEN];
    struct sockaddr_in6 *in6 = (struct sockaddr_in6 *)(addr);
    inet_ntop(AF_INET6, &(in6->sin6_addr), astring, INET6_ADDRSTRLEN);
    printf("%s\n", astring);

  } else {
    printf(" ???? address and port ???? \n");
  }

  *ret_fd = socket_fd;
  return 1;
}
