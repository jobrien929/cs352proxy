#include "tap.h"
#include "socket.h"
#include <pthread.h>
#include <semaphore.h>

int main(int argc, char ** argv){
		//Determins the port of the remote server and local server
		//If device is the client the local port is the same as the the remote port
		//since the local port isn't specified
		//ie. port on server = port on this device
		int port;
		if (argc == 2){
			port = atoi(argv[1]);
		} else if (argc == 3){
			port = atoi(argv[2]);
		} else {
			printf("Error: You entered something incorrectly for the arguments");
			exit(1);
		}
		
		
		
	
	
		//opens the tap
		//exits entire program upon failure to open tap
		int tapID;					//Id / handle of the tap
		if (argc ==2){
			tapID = openTap(argv[2]);
		}else{
			tapID = openTap(argv[3]);
		}
		
		
		
		
		
		
		//if device is server then it waits for connection and accepts it
		//else it establishes a connection
		int commSocket;
		if(argc == 2){		//If device is server
			commSocket = acceptConnection(port);
		} else{		//If device is client
			//Checks if the 3rd argument is in DNS format or IPV4
			char *IP = malloc(sizeof(char)*500);
			char *IPReal = IP;
			if(atoi(argv[3])==0){
				//Converts from DNS format to IPV4
				struct hostent *converter;
				converter = gethostbyname(argv[3]);
				inet_ntop(converter->h_addrtype,converter->h_name,IP,INET_ADDRSTRLEN);
			}else{
				IPReal = argv[3];
			}
			commSocket = socket(AF_INET, SOCK_STREAM, 0);
			if(connectToServer(commSocket, port, IPReal) == 0){
					printf("ERROR: Could not connect to server exiting program");
					exit(1);
			}
			free(IP);
		}
		
		//creates the mutexes
		pthread_mutex_init(&socketMu, NULL);
		pthread_mutex_init(&tapMu, NULL);
		
		//thread for reading from tap and sending msg through socket
		int argt[2];
		argt[0]=tapID;
		argt[1]=commSocket;
		pthread_create(&threadtapRID, NULL,(void *)readTap,argt);
		
		//thread for reading from socket and writing to tap
		int args[2];
		args[0]=commSocket;
		args[1]=tapID;
		pthread_create(&threadsockRID, NULL,(void *)writeTap,args);
		
		int rt;
		pthread_join(threadtapRID,(void *)(&rt));
		if(rt==1){
			printf("There was an error from reading from tap");
		}
		
		int wt;
		pthread_join(threadsockRID,(void *)(&wt));
		if(wt==1){
			printf("There was an error from writing to tap");
		}		
		
		//Closes Socket and tap
		close(tapID);
		close(commSocket);
		
		
}
