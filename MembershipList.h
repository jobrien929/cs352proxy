#include <stdio.h>
#include <string.h>
#include "linkedlist.h"
#include <time.h>
#include <string.h>
#include <pthread.h>

/**Node structure in linklist that holds each member of the membership
 * list**/
typedef struct Node{
	char * memberName;		//name of member; can be decimal format or DNS name
	int timeEntered;				//number of seconds after January 1, 1970 at midnight member was discoverd
	int timeOut;			//seconds from time stored member should expire
}memberNode;

/**Creates a membershipList and returns what it has created*/
linkedListHolder membershipListCreate();


/**Adds member to membership list
 * Name - Name(IP address or DNS name [keep consistent you will
 * need to use the same exact name when removing it from list])  user 
 * wishes to remove
 * timeIn - number of seconds after January 1, 1970 at midnight new member was discovered;
 *    use time() function to get this value
 * returns: 0 on success 1 on failure
 * NOTE: user is responsible for marking down when new member was discovered as soon as it
 * is discovered!!!**/ 
int addMember(char *Name, int expires, time_t timeIn, linkedListHolder membershipList);

/**removes member from membership list, but does not use the linkedList
 * function to do this for various reasons
 * memberName - Name (use the exact format used to store the member) of
 * memberNode that user wants to remove;
 * membershipList - list containing each proxy in the VLAN
 * returns:0 on success 1 on failure**/
int removeMember(char *Name, linkedListHolder membershipList);

/**removes all members from the membership list that is expired
 * membershipList - list of proxies within VLAN
 * returns: number of removed proxies**/
int removeExpired(linkedListHolder membershipList);

