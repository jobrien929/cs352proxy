#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/if_tun.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>

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
void openTap(){
	char *if_name = (char*)malloc(sizeof(char) * 16);
	strcpy(if_name, "tap0");
	int tap_fd;
	
	if ( (tap_fd = allocate_tunnel(if_name, IFF_TAP | IFF_NO_PI)) < 0 ) {
		perror("Opening tap interface failed! \n");
		exit(1);
	}
	
	return tap_fd;
}

/**Continuously reads from the tap and returns the string it gets from tap
 * Takes the handle of the tap as a parameter for tap_fd
 * Will take up to 500 bytes from tap otherwise segmentation fault,
 *    but the 500 bytes is adjustable
 * User must remember to free the return
 * Also assumes tap was already opened*/
char *readTap(int tap_fd){
	//Malloc for the string that is taken from the tap (takes up to 500 bytes)
	char *tapEntry = (char*)malloc(sizeof(char) * 500); 
	
	while(1){
		if(read(tap_fd, tapEntry,500) < 0){
			printf("Error: Could not read from tap\n");
			exit(1);
		}
	}
	
	return tapEntry
}

/**Writes messages into the tap
 * msg = message that needs to be written to the tap
 * tap_fd = the handle of the tap
 Assumes the tap was already open*/
void writeTap(char *msg, int tap_fd){


	if(write(tap_fd, msg, sizeof(msg)) < 0){
		printf("Error: Could not read from tap\n");
		exit(1);
	}
	
}
