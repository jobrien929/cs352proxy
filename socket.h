#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>


int openSocket(int port);
int connectToServer(int sock, int port, char * name);


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