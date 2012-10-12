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
#define HEADER_SIZE 4

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

void* outgoing(void* sockets) {

  char msg[PAYLOAD_SIZE + HEADER_SIZE];
  ushort msg_length;
  int sent, sent_total;
  ushort *len_ptr, *type_ptr;
  struct socks_h *socks = (struct socks_h*)sockets;

  while(1) {
    if((msg_length = read(socks->tun_sock, msg+HEADER_SIZE, PAYLOAD_SIZE)) < 0) {
      perror("Outgoing recv");
      exit(EXIT_FAILURE);
    }
    type_ptr = (ushort*)msg;
    len_ptr = (ushort*)msg+2;
    *type_ptr = htons(0xabcd);
    *len_ptr = htons(msg_length);
    printf("Received %u bytes from tap\ttype %x\n", ntohs(*len_ptr), ntohs(*type_ptr));
    msg_length += HEADER_SIZE;
    sent_total = 0;
    do {
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

void* incoming(void* sockets) {

  char msg[PAYLOAD_SIZE];
  ushort type, msg_length, *type_ptr, *len_ptr;
  int sent, sent_total;
  struct socks_h *socks = (struct socks_h*)sockets;

  while(1) {
    readn(socks->remote_sock, msg, HEADER_SIZE);  // Read next header
    type_ptr = (ushort*)msg;
    len_ptr = (ushort*)msg+2;
    type = ntohs(*type_ptr);
    msg_length = ntohs(*len_ptr);
    printf("Received header from tunnel: type %x\tlength: %u \n", type, msg_length);
    readn(socks->remote_sock, msg, msg_length);
    sent_total = 0;
    do {
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
  printf("argc=%d\n", argc);
  if(argc < 3 || argc > 4) {
    printf("Invalid arguments.\n");
  }

/* The main thread reads the command line arguments */
/* Create a TCP socket on the port as defined in the command line. */
// Set up structures for getaddrinfo
  struct addrinfo hints, *res;
  int status, connect_sock;
  char *addr;
  char *port;
  struct socks_h socks;
  pthread_t  tid_incoming, tid_outgoing;

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

    socks.remote_sock = connect_sock;

    printf("Successful connection to server\n");
    
  }

if ( (socks.tun_sock = allocate_tunnel(argv[argc-1], IFF_TAP | IFF_NO_PI)) < 0 ) {
    perror("Opening tap interface failed! \n");
    exit(1);
  }

  pthread_create(&tid_outgoing, NULL, outgoing, (void *)&socks);
  pthread_create(&tid_incoming, NULL, incoming, (void *)&socks);

  pthread_join(tid_incoming, NULL);
  pthread_join(tid_outgoing, NULL);






  freeaddrinfo(res);
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
