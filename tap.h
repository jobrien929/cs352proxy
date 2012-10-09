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
int allocate_tunnel(char *dev, int flags);

/**Continuously reads from the tap and returns the string it gets from tap
 * Takes the handle of the tap as a parameter for tap_fd
 * Will take up to 500 bytes from tap otherwise segmentation fault,
 *    but the 500 bytes is adjustable
 * User must remember to free the return
 * Also assumes tap was already opened*/
char *readTap(int tap_fd);

/**opens the tap connection
 * returns the tap handle
 * DONT FORGET TO CLOSE TAP using close(tap_fd)*/
int openTap(char * tapName);

/**Writes messages into the tap
 * msg = message that needs to be written to the tap
 * tap_fd = the handle of the tap
 Assumes the tap was already open*/
void writeTap(char *msg, int tap_fd);


