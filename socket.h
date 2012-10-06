#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <pthread.h>


int (* functionPointer) (int) = NULL; /*generic function pointer, accepts a socket*/

int openSocket(int port); /*This creates a generic socket, that performs asynchronously, using whatever address the machine wants to give it, on the port given*/
int connectToServer(int sock, int port, char * name); /*creates an outgoing connection, using the given socket, the port of the away machine, and the IP address (e.g. 198.162.2.2) of the remote machine. Returns 0 on failure, 1 on success*/
int acceptConnection(int sock, int(* functionPointer)(int)); /*accepts an inbound connection on the specified socket, with a function for it to call upon accept. This function should not return unless broken from, or gives an error (such as a fail listen or fail accept). Upon accepting the new connection, a new socket is created to work with.*/

/**Continuously reads from the tap and prints the string it gets from tap
 * Takes no parameters
 * Will take up to 500 bytes from tap otherwise segmentation fault,
 *    but the 500 bytes is adjustable
 * Exits entire program if for some reason program can't open tap*/
char * read();

/**Writes messages into the tap
 * There are no parameters
 Exits entire program if for some reason program can't open tap*/
char * write(char *msg);