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
#include<stdio.h>
#include<net/if.h>
#include<string.h>

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

int main(int argc, char* argv[]) {
  
  if(argc < 2 || argc > 3) {
    printf("Invalid arguments.\n");
  }

/* The main thread reads the command line arguments */
/* Create a TCP socket on the port as defined in the command line. */
/* create a thread to handle virtual tap device */
/* create a thread to handle incoming packets on the TCP socket */
/* Run the thread for the TCP socket, which: */
/*         If it is the server mode proxy, listen on the port, else */
/*                 connect to the other proxy */
/*         Loop forever: */
/*         listen for packets from the TCP socket */
/*                 when a packet arrives, de-encapsulate the packet */
/* send it to the local tap device */
 
/* Run the thread for the  tap device, which: */
/*         Loop forever: */
/*         Listen for a packet from the tap device */
/*                 Encapsulate the packet in the proper format */
/*                 Send the packet to over the TCP socket */
}
