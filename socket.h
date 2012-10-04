#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>


int openSocket(int port);
int connectToServer(int sock, int port, char * name);
