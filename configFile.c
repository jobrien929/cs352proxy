#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//This is a tokenizer file I made for 214 and needed it for this
//program, so this program would rely on tokenizer.c
#include "tokenizer.h"
#include "linkedlist.h"

//MACROS that should be added into header
#define TAPDEVICESIZE 10

//Global variables that should be added into header
char tapDevice[TAPDEVICESIZE];
int linkPeriod;
int linkTimeout;
int quitAfter;


/**readConfigFile reads the configuration file required to run the VLAN
 * program.
 * Parameters: 
 * 	FILE *configFile - pointer to the configuration file, it should
 * 		already be opened, if NULL error will return.
 *  linkedListHolder peer - pointer to a linklist of peers, if NULL creates
 * 		a link list of peers
 * Return: 0 on success, 1 on failure 
 * NOTE: Lines that are longer than 279 characters will be cut short
 * NOTE: Also assumes text file was created using linux OS**/
int readConfigFile(FILE *configFile, linkedListHolder peer){
	if (configFile == NULL){
		printf("Error: Bad Config File\n");
		return 1;
	}
	
	//reads a line from file
	char *buffer = (char*)malloc(sizeof(char)*280);
	fgets(buffer, 280,configFile);
	
	//Sees if file is blank
	if(strlen(buffer) == 0 && ftell(configFile)){
		printf("Error: File is blank\n");
		return 1;
	}
   int keyC=0;
	while(strlen(buffer) != 0){
		//Sees if file read entire line
		int position = ftell(configFile);
		position--;
		fseek(configFile, position, SEEK_SET);
		char breakBool = fgetc(configFile);
		if (breakBool != '\n' && breakBool != EOF){
			
			//Reads from file until next line
			breakBool  = fgetc(configFile);
			while(breakBool != '\n'&& breakBool != EOF){
				breakBool = fgetc(configFile);
			}
		}
	
		//Checks if line is only 1 character long
		if(strlen(buffer)==1){
			printf("Error: One of the lines in the file is in incorrect format\n");
			return 1;
		}
		
		//Checks if line is a comment
		if(buffer[0] == '/' && buffer[1] == '/'){
			memset(buffer, '\0', sizeof(buffer));
			fgets(buffer, 280, configFile);
			continue;
		}
		
		TokenizerT words = TKCreate(" ",buffer);
		//Sees if last charcter in token is '\n' if so it removes it
		if(((words->next)->token)[strlen(((words->next)->token))-1] == '\n'){
			((words->next)->token)[strlen(((words->next)->token))-1] = ((words->next)->token)[strlen(((words->next)->token))];
		}
		if(strcmp((words->token),"tapDevice") == 0){
			memcpy(tapDevice,(words->next)->token, strlen((words->next)->token));
		}else if(strcmp((words->token),"linkPeriod") == 0){
			linkPeriod = atoi((words->next)->token);
		}else if(strcmp((words->token),"linkTimeout") == 0){
			linkTimeout = atoi((words->next)->token);
		}else if(strcmp((words->token),"peer") == 0){
			char *peerName = (char*)malloc(sizeof(char)*strlen((words->next)->token));
			memcpy(peerName,(words->next)->token,strlen((words->next)->token)+1);
			add(peer, peerName, keyC);
         keyC++;
		}else if(strcmp((words->token),"quitAfter") == 0){
			quitAfter = atoi((words->next)->token);
		}else{
			printf("Error: One of the lines in the file is in incorrect format\n");
			return 1;
		}
		
		//Cleanup
		memset(buffer, '\0', sizeof(buffer));
		fgets(buffer, 280, configFile);
		
	}
	
	//Cleanup
	free(buffer);
	
	return 0;

}

