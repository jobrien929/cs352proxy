#include "tap.h"
#include "socket.h"



/**************************************************
 * allocate_tunnel: 
 * open a tun or tap device and returns the file
 * descriptor to read/write back to the caller
 *****************************************/
int allocate_tunnel(char *dev, int flags) {
	int fd, error;
	struct ifreq ifr;
	char *device_name = "/dev/net/tun"; 
	if( (fd = open(device_name , O_RDWR)) < 0 ) {
		perror("error opening /dev/net/tun");
		return fd;
	}
	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = flags;
	if (*dev) {
		strncpy(ifr.ifr_name, dev, IFNAMSIZ);
	}
	if( (error = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0 ) {
		perror("ioctl on tap failed");
		close(fd);
		return error;
	}

	strcpy(dev, ifr.ifr_name);
	return fd;
}

/**opens the tap connection
 * returns the tap handle
 * DONT FORGET TO CLOSE TAP using close(tap_fd)*/
int openTap(char * tapName){
	char *if_name = (char*)malloc(sizeof(char) * 16);
	strcpy(if_name, tapName);
	int tap_fd;
	
	if ( (tap_fd = allocate_tunnel(if_name, IFF_TAP | IFF_NO_PI)) < 0 ) {
		perror("Opening tap interface failed! \n");
		exit(1);
	}
	
	free(if_name);
	
	return tap_fd;
}

/**Continuously reads from the tap and sends what it read into the socket
 * Only returns if something goes wrong otherwise loops forever
 * Takes the handle of the tap as a parameter for tap_fd
 * Will take up to 500 bytes from tap otherwise segmentation fault,
 *    but the 500 bytes is adjustable
 * Also assumes tap was already opened*/
int readTap(int tap_fd, int socketID){
	datagram d;
	fd_set readfds;
	
		
	while(1){
		//Waits till tap is ready to be read from
		FD_ZERO(&readfds);
		FD_SET(tap_fd, &readfds);
		select(tap_fd+1, &readfds, NULL, NULL, NULL);
		
		//reads from tap
		pthread_mutex_lock(&tapMu);			//Locks tap for only reading from tap
		short nbits = read(tap_fd, d.data,2044);
		pthread_mutex_unlock(&tapMu);		//Unlocks tap
		
		if(nbits < 0){
			printf("Error: Could not read from tap\n");
			close(tap_fd);
			return 1;
		}
		//Headers for file
		//type:
		short vlanID = 0xABCD;
		d.type = htons(vlanID);
		//length: (this is taken in bytes)
		d.length = htons(nbits);
		
		//Sends data over the socket
		pthread_mutex_lock(&socketMu);			//Locks socket for only write
		send(socketID, &d, d.length, 0);
		pthread_mutex_unlock(&socketMu);		//unlocks socket
	}
	
	
	return 0;
}

/**Writes messages into the tap
 * socketID = socket to wait for message
 * tap_fd = the handle of the tap
 * Returns a 1 upon error and returns a 0 upon success
 Assumes the tap was already open*/
int writeTap(int socketID, int tap_fd){
	fd_set readfds;
	datagram *msg =(datagram *)malloc(sizeof(datagram));
	
	while(1){
		//waits to receive message from socket
		FD_ZERO(&readfds);
		FD_SET(tap_fd, &readfds);
		select(tap_fd+1, &readfds, NULL, NULL, NULL);
		
		//recieves msg from socket
		pthread_mutex_lock(&socketMu);		//locks socket for read
		recv(socketID, msg, 500, 0); 
		pthread_mutex_unlock(&socketMu);		//unlocks socket
		
		//Strips header from msg
		int length = ntohs(msg->length);
		
		char strippedMsg[500];
		memcpy(strippedMsg, msg->data, length); 
		
		//Sends data through tap
		pthread_mutex_lock(&tapMu);			//locks tap for write
		int writeError = write(tap_fd, msg->data, length);
		pthread_mutex_unlock(&tapMu);		//unlocks tap
		
		if( writeError< 0){
			printf("Error: Could not write to tap\n");
			return 1;
		}
	}
	
	return 0;
	
}
