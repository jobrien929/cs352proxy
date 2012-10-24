
int openSocket(int port) {
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in myaddr;
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(port);
	bind(sock, (struct sockaddr *)&myaddr, sizeof(myaddr));
	fcntl(sock, F_SETFF, FNDELAY); /*THIS SOCKET WILL NOW PERFORM ASYNCHRONOUSLY*/
	return sock;
}

int connectToServer(int sock, int port, char * name) {
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
	
