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

/**Continuously reads from the tap and prints the string it gets from tap
 * Takes no parameters
 * Will take up to 500 bytes from tap otherwise segmentation fault,
 *    but the 500 bytes is adjustable
 * Exits entire program if for some reason program can't open tap*/
char * read(){
	char *if_name = (char*)malloc(sizeof(char) * 16);
	strcpy(if_name, "tap0");
	int tap_fd;
	//Malloc for the string that is taken from the tap (takes up to 500 bytes)
	char *tapEntry = (char*)malloc(sizeof(char) * 500); 
	
	
	if ( (tap_fd = allocate_tunnel(if_name, IFF_TAP | IFF_NO_PI)) < 0 ) {
		perror("Opening tap interface failed! \n");
		exit(1);
	}

	read(tap_fd, tapEntry,500);
	
	//free
	free(tapEntry);
}

/**Writes messages into the tap
 * There are no parameters
 Exits entire program if for some reason program can't open tap*/
char * write(char *msg){
	char *if_name = (char*)malloc(sizeof(char) * 16);
	strcpy(if_name, "tap0");
	int tap_fd;
	
	
	if ( (tap_fd = allocate_tunnel(if_name, IFF_TAP | IFF_NO_PI)) < 0 ) {
		perror("Opening tap interface failed! \n");
		exit(1);
	}

	write(tap_fd, msg, sizeof(msg));

}
