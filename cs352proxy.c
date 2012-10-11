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
#include <stdio.h>
#include <net/if.h>
#include <netdb.h>
#include <string.h>

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
  printf("argc=%d\n", argc);
  if(argc < 3 || argc > 4) {
    printf("Invalid arguments.\n");
  }

/* The main thread reads the command line arguments */
/* Create a TCP socket on the port as defined in the command line. */
// Set up structures for getaddrinfo
  struct addrinfo hints, *res;
  int status;
  char *addr;
  char *port;
  int tcp_sock, rw_sock, tun_sock;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET; // Use IPv4
  hints.ai_socktype = SOCK_STREAM; //Use tcp
  if(argc == 3) {  // I'm a server
    printf("I'm a server\n");
    hints.ai_flags = AI_PASSIVE;
    addr = NULL;
    port = argv[1];
  } else { // I'm a client
    printf("I'm a client\n");
    addr = argv[1];
    port = argv[2];
  }

  if((status = getaddrinfo(addr, port, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return(EXIT_FAILURE);
  }

  if((tcp_sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
    perror("Getting socket");
    return(EXIT_FAILURE);
  }

  if(argc == 3) { // I'm a server

    if(bind(tcp_sock, res->ai_addr, res->ai_addrlen) < 0) {
      perror("Bind");
      return(EXIT_FAILURE);
    }

    if(listen(tcp_sock,2) < 0) {
      perror("Listen");
      return(EXIT_FAILURE);
    }

    if((rw_sock = accept(tcp_sock, NULL, NULL)) < 0) {
      perror("Accept");
      return(EXIT_FAILURE);
      }

    printf("Successful connection from client\n");

  } else { // I'm a client

    if(connect(tcp_sock, res->ai_addr, res->ai_addrlen) < 0) {
      perror("Connect");
      return(EXIT_FAILURE);
    }

    printf("Successful connection to server\n");

    rw_sock = tcp_sock;
    
  }

  freeaddrinfo(res);
  close(rw_sock);
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
  return EXIT_SUCCESS;
}
