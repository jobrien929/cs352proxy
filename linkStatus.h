#include <sys/time.h>

struct linkStatus {
	int socket; //probably not needed
	char * myIP;
	char * serverIP;
	time_t lastTime;
	char * myMAC;
	char * serverMac;
	int myPort;
	int serverPort;
	double RTT;
	int sequenceNumber;
};
	
