#include "socket.h"

int openSocket(int port) { /*This creates a generic socket, using whatever address the machine wants to give it, on the port given*/
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in myaddr;
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(port);
	bind(sock, (struct sockaddr *)&myaddr, sizeof(myaddr));
	//fcntl(sock, F_SETFF, FNDELAY); /*THIS SOCKET WILL NOW PERFORM ASYNCHRONOUSLY*/
	return sock; /*returns the FD of the socket*/
}

int connectToServer(int sock, int port, char * name) { /*creates an outgoing connection, using the given socket, the port of the away machine, and the IP address (e.g. 198.162.2.2) of the remote machine. Returns 0 on failure, 1 on success*/
	struct sockaddr_in servaddr;
	memset((char *_)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = inet_addr(name);
	
	if (connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		return 0;
	}
	return 1;
}

int acceptConnection(int sock) { //accepts an inbound connection on the specified socket. This accepts ONLY 1 connection request and returns the socket which does the communication on succes, exits the entire program upon failure
	socklen_t alen;
	struct sockaddr_in my_addr;
	struct sockaddr_in client_addr;
	int sockoptval = 1;
	int acceptedsocket;
	
	if (listen(svc, 10) < 0) {
		perror("listen failed");
		exit(1);
	}
	
	acceptedsocket = accept(sock, (struct sockaddr *)&client_addr, &alen);
	
	if(acceptsocket < 0){
		perror("accept failed");
		exit(2);
	}
	return acceptsocket;		
}
