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

#define PAYLOAD_SIZE 2044
#define TYPE_SIZE 2
#define LENGTH_SIZE 2
#define HEADER_SIZE (TYPE_SIZE + LENGTH_SIZE)


/* Structure for sending socket file descriptors to input  */
/* and output threads */ 
struct socks_h {
  int tun_sock;
  int remote_sock;
};

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


/* Reads exactly length bytes from fd into *buf. */
/* Does not return until length bytes have been read. */
void readn(int fd, char *buf, int length) {
  int received;
  int received_total = 0;
  do {
    if((received = read(fd, buf+received_total, length - received_total)) < 0) {
      perror("readn");
      exit(EXIT_FAILURE);
    }
    received_total += received;
  } while(received_total < length);

  return;
}


/* Function to run thread that receives data from the tap, */
/* encapsulates it, and sends it thru the tunnel. sockets should
   be a pointer to a socks_h structure holding the socket 
   file descriptors. */
void* outgoing(void* sockets) {

  char msg[PAYLOAD_SIZE + HEADER_SIZE];
  int  msg_length;
  ushort type = 0xabcd;
  int sent, sent_total;
  ushort *len_ptr, *type_ptr;
  struct socks_h *socks = (struct socks_h*)sockets;

  while(1) {  //keep checking forever
    
    // Get data from tap
    if((msg_length = read(socks->tun_sock, msg+HEADER_SIZE, PAYLOAD_SIZE)) < 0) {
      perror("Outgoing recv");
      exit(EXIT_FAILURE);
    }

    // Encapsulate data
    type_ptr = (ushort*)msg;
    len_ptr = (ushort*)(msg+TYPE_SIZE);
    *type_ptr = htons(type);
    *len_ptr = htons(msg_length);

    printf("Received %u bytes from tap\ttype %x\n", ntohs(*(ushort*)(msg+2)), ntohs(*(ushort*)msg));
    
    // Send packet thru tunnel
    msg_length += HEADER_SIZE;
    sent_total = 0;
    do { // Loop until the entire packet has been sent
      if((sent = write(socks->remote_sock, msg+sent_total, msg_length - sent_total)) < 0) {
	perror("Outgoing send");
	exit(EXIT_FAILURE);	
      }
      sent_total += sent;
    } while(sent_total < msg_length);

    printf("Sent %d bytes to tunnel\n", sent_total);

  }
  printf("Exiting outgoing thread\n");
  return NULL;
}


/* Function to run thread that receives packets from the tunnel,
   strips off the header, and sends the to the tap.  sockets should
   be a pointer to a socks_h structure holding the socket 
   file descriptors. */
void* incoming(void* sockets) {

  char msg[PAYLOAD_SIZE];
  ushort type, msg_length;
  int sent, sent_total;
  struct socks_h *socks = (struct socks_h*)sockets;

  while(1) {  // Loop forever

    // Get next header
    readn(socks->remote_sock, msg, HEADER_SIZE);

    // Get packet type and length
    type = ntohs(*(ushort*)msg);
    msg_length = ntohs(*(ushort*)(msg+TYPE_SIZE));
    printf("Received header from tunnel: type %x\tlength: %u \n", type, msg_length);
    
    // Get packet payload
    readn(socks->remote_sock, msg, msg_length);
    
    // Send payload to tap
    sent_total = 0;
    do { // Keep sending until entire payload has been sent
      if((sent = write(socks->tun_sock, msg+sent_total, msg_length - sent_total)) < 0) {
	perror("Incoming send");
	exit(EXIT_FAILURE);
      }
      sent_total += sent;
    } while(sent_total < msg_length);

    printf("Sent %d bytes to tap\n", sent_total);

  }
  printf("Exiting outgoing thread\n");

  return NULL;
}

int main(int argc, char* argv[]) {

  // Check for correct syntax
  printf("argc=%d\n", argc);
  if(argc < 3 || argc > 4) {
    printf("Invalid arguments.\n");
  }


  struct addrinfo hints, *res;
  int status, connect_sock;
  char *addr;
  char *port;
  struct socks_h socks;
  pthread_t  tid_incoming, tid_outgoing;

  // Set up structure for getaddrinfo
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

  // getaddrinfo will populate res with the connection info
  if((status = getaddrinfo(addr, port, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return(EXIT_FAILURE);
  }

  // Create socket
  if((connect_sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
    perror("Getting socket");
    return(EXIT_FAILURE);
  }

  if(argc == 3) { // I'm a server

    if(bind(connect_sock, res->ai_addr, res->ai_addrlen) < 0) {
      perror("Bind");
      return(EXIT_FAILURE);
    }
 
    if(listen(connect_sock,2) < 0) {
      perror("Listen");
      return(EXIT_FAILURE);
    }

    // Put file descriptor for the connection into the socks struct
    if((socks.remote_sock = accept(connect_sock, NULL, NULL)) < 0) {
      perror("Accept");
      return(EXIT_FAILURE);
    }

    printf("Successful connection from client\n");

  } else { // I'm a client

    if(connect(connect_sock, res->ai_addr, res->ai_addrlen) < 0) {
      perror("Connect");
      return(EXIT_FAILURE);
    }

    // Put file descriptor for the connection into the socks struct
    socks.remote_sock = connect_sock;

    printf("Successful connection to server\n");
    
  }

  // Put file descriptor for the tap into the socks struct
  if ( (socks.tun_sock = allocate_tunnel(argv[argc-1], IFF_TAP | IFF_NO_PI)) < 0 ) {
    perror("Opening tap interface failed! \n");
    exit(1);
  }

  // Free the addrinfo structure
  freeaddrinfo(res);


  // Create thread to process outgoing data
  pthread_create(&tid_outgoing, NULL, outgoing, (void *)&socks);
  // Create thread to process incoming data  
  pthread_create(&tid_incoming, NULL, incoming, (void *)&socks);

  // These might be useful if the threads didn't loop forever
  pthread_join(tid_incoming, NULL);
  pthread_join(tid_outgoing, NULL);


  return EXIT_SUCCESS;
}
