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
#include <netinet/in.h>
#include <unistd.h>

pthread_t threadtapRID;			//Thread from reading from tap
pthread_t threadsockRID;		//Thread for reading from socket
pthread_mutex_t socketMu;		//lock for socket
pthread_mutex_t tapMu;			//lock for tap

/**************************************************
 * allocate_tunnel: 
 * open a tun or tap device and returns the file
 * descriptor to read/write back to the caller
 *****************************************/
int allocate_tunnel(char *dev, int flags);

typedef struct _datagram{
	short type;
	short length;
	char data[2044];
} datagram;

/**Continuously reads from the tap and sends what it read into the socket
 * Only returns if something goes wrong otherwise loops forever
 * Takes the handle of the tap as a parameter for tap_fd
 * Will take up to 500 bytes from tap otherwise segmentation fault,
 *    but the 500 bytes is adjustable
 * Also assumes tap was already opened*/
int readTap(int tap_fd, int socketID);

/**opens the tap connection
 * returns the tap handle
 * DONT FORGET TO CLOSE TAP using close(tap_fd)*/
int openTap(char * tapName);

/**Writes messages into the tap
 * socketID = socket to wait for message
 * tap_fd = the handle of the tap
 * Returns a 1 upon error and returns a 0 upon success
 Assumes the tap was already open*/
int writeTap(int socketID, int tap_fd);




