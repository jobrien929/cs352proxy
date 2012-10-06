
int openSocket(int port) { /*This creates a generic socket, that performs asynchronously, using whatever address the machine wants to give it, on the port given*/
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in myaddr;
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(port);
	bind(sock, (struct sockaddr *)&myaddr, sizeof(myaddr));
	fcntl(sock, F_SETFF, FNDELAY); /*THIS SOCKET WILL NOW PERFORM ASYNCHRONOUSLY*/
	return sock; /*returns the FD of the socket*/
}

int connectToServer(int sock, int port, char * name) { /*creates an outgoing connection, using the given socket, the port of the away machine, and the IP address (e.g. 198.162.2.2) of the remote machine. Returns 0 on failure, 1 on success*/
	struct sockaddr_in servaddr;
	memset((char *_)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	
	memcpy((void *)&servaddr.sin_addr, name, sizeof(name));
	
	if (connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		return 0;
	}
	return 1;
}

int acceptConnection(int sock, int(* functionPointer)(int)) { /*accepts an inbound connection on the specified socket. This function should not return unless broken from, or gives an error (such as a fail listen or fail accept). Upon accepting the new connection, a new socket is created to work with.*/
	socklen_t alen;
	struct sockaddr_in my_addr;
	struct sockaddr_in client_addr;
	int sockoptval = 1;
	int acceptedsocket;
	
	if (listen(svc, 10) < 0) {
		perror("listen failed");
		return -1;
	}
	
	while (1) {
		while ((acceptedsocket = accept(sock, (struct sockaddr *)&client_addr, &alen)) < 0) {
			if ((errno != ECHILD) && (errno != ERESTART) && (errno != EINTR)) {
				perror("accept failed");
				return -2;
			}
			/*INSERT NEW THREAD HERE FOR THE SOCKET TO TALK TO - USE ACCEPTEDSOCKET*/
			/*JOHN FINISH THIS OUT, USE THE FUNCTIONPOINTER PASSED INTO ACCEPTCONNECTION TO SPAWN THE THREAD*/
		}
	}
	return 0;
}
