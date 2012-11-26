//NOTE:Though these functions are NOT thread functions, thread functions
//may call these functions and there is a mutex lock called membershipListMutex
//that locks down the membershipList when it is in use

#include "MembershipList.h"


/**Creates a membershipList and returns what it has created*/
linkedListHolder membershipListCreate(){
	return create();
}

/**Adds member to membership list, if membershipList is NULL it fails
 * Name - Name(IP address or DNS name [keep consistent you will
 * need to use the same exact name when removing it from list])  user 
 * wishes to remove
 * timeIn - number of seconds after January 1, 1970 at midnight new member was discovered;
 *    use time() function to get this value
 * membershipList - list of members we are adding, must be created using membershipLIstCreate first
 * returns: 0 on success 1 on failure
 * NOTE: user is responsible for marking down when new member was discovered as soon as it
 * is discovered!!!**/ 
int addMember(char *Name, int expires, time_t timeIn,linkedListHolder membershipList){
	//membershipList was not created
	if(membershipList == NULL){
		printf("Error: User sent in a bad membershipList\n");
		return 1;
	}
	
	//Creates member
	memberNode *newMember = (memberNode *)malloc(sizeof(memberNode));
	newMember->memberName = Name;
	newMember->timeEntered = timeIn;
	newMember->timeOut = timeIn + expires;
	
	//Adds member to membershipList
	pthread_mutex_lock(&membershipListMutex);
	add(membershipList, newMember, newMember->timeOut);
	pthread_mutex_unlock(&membershipListMutex);
	
	return 0;
}

/**removes member from membership list, but does not use the linkedList
 * function to do this for various reasons
 * memberName - Name (use the exact format used to store the member) of
 * memberNode that user wants to remove;
 * membershipList - list containing each proxy in the VLAN
 * returns:0 on success 1 on failure**/
int removeMember(char *Name, linkedListHolder membershipList){
	
	pthread_mutex_lock(&membershipListMutex);
	if (membershipList==NULL){
		printf("Error: User sent in a bad membershipList\n");
		return 1;
	}
	
	linkedList ptr = membershipList->head;
	if (ptr==NULL){
		printf("Error: List is empty\n");
		return 1;
	}
	do{
		if(strcmp(Name, ((memberNode*)ptr->data)->memberName)==0){
			if(membershipList->head->next == membershipList->head){
				free((memberNode*)ptr->data);
				removeNode(membershipList, ptr->key);
				return 0;
			}
			ptr->prev->next = ptr->next;
			ptr->next->prev = ptr->prev;
			if(ptr == membershipList->head){
				membershipList->head = membershipList->head->next;
			}
			free(ptr);
			return 0;
		}
		ptr = ptr->next;
	}while(ptr!=membershipList->head);
	pthread_mutex_unlock(&membershipListMutex);
	
	printf("Error: Could not find member in the membership list\n");
	return 1;
}

/**Endlessly and reptedly checks every member in the membershipList
 * and removes all expired members
 * membershipList - list of proxies within VLAN
 * returns: number of removed proxies**/
int removeExpired(linkedListHolder membershipList){
	pthread_mutex_lock(&membershipListMutex);
	if (membershipList==NULL){
		printf("Error: User sent in a bad membershipList\n");
		return 0;
	}
	
	linkedList ptr = membershipList->head;
	time_t currentTime;
	currentTime = time(NULL);
	int count = 0;
	do{
		if(currentTime > ptr->key){
			if(membershipList->head->next == membershipList->head){
				free((memberNode*)ptr->data);
				removeNode(membershipList, ptr->key);
				return 1;
			}
			ptr->prev->next = ptr->next;
			ptr->next->prev = ptr->prev;
			if(ptr == membershipList->head){
				membershipList->head = membershipList->head->next;
			}
			linkedList temp = ptr;
			ptr = ptr->next;
			free((memberNode*)temp->data);
			free(temp);
			count++;
			continue;
		}
		ptr = ptr->next;
	}while(ptr!=membershipList->head);
	pthread_mutex_unlock(&membershipListMutex);
	return count;
}
