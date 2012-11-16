

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tokenizer.h"



/*
 * TKCreate creates a new TokenizerT object for a given set of separator
 * characters (given as a string) and a token stream (given as a string).
 * 
 * TKCreate should copy the two arguments so that it is not dependent on
 * them staying immutable after returnning.  (In the future, this may change
 * to increase efficiency.)
 *
 * If the function succeeds, it returns a non-NULL TokenizerT.
 * Else it returns NULL.
 *
 * You need to fill in this function as part of your implementation.
 */

TokenizerT TKCreate(char *separators, char *ts) {
	
	/*Tests to make sure separators and ts exists*/
	if (separators==NULL || ts==NULL) {
		return NULL;
	}
	
	char *sep;								/*seperator characters*/
		sep = malloc(sizeof(char)*(strlen(separators)+1));
	char *tokenString;						/*String your trying to seperate*/
		tokenString = malloc(sizeof(char)*(strlen(ts)+1));
	int length;								/*length of each token*/
	int charptrTS;							/*points to the char location in the tokenString*/
	TokenizerT rtrn=NULL;					/*return tokenizer*/
	TokenizerT rtrnPTR	= NULL;				/*location of latest node in the returning tokenizer*/	
	char *begins;							/*points to the beginning of the token*/
	
	/*copies sperators and ts to sep and tokenString*/
	memcpy(sep,separators,((strlen(separators)+1)*sizeof(char)));
	memcpy(tokenString,ts,((strlen(ts)+1)*sizeof(char)));
	
	/*starts to seperate tokenString*/
	length=0;
	charptrTS=0;
	begins=&tokenString[0];
	
	while (charptrTS!= strlen(tokenString)){
		int charptrSEP=0;
		for (; charptrSEP < strlen(sep); charptrSEP++) {
			if (tokenString[charptrTS]==sep[charptrSEP]){
				if (length==0){
					begins=&tokenString[charptrTS+1];
					break;
				}else {
					TokenizerT temp = malloc(sizeof(struct TokenizerT_));
					char *String= malloc(sizeof(char)*(length+1));
					strncpy(String,begins,length);
					String[length]='\0';
					temp->token=String;	
					temp->next=NULL;
					if (rtrn==NULL){
						rtrn=temp;
					} else {
						rtrnPTR->next=temp;
					}
					rtrnPTR=temp;
				}
				begins=&tokenString[charptrTS+1];
				length=0;
			}else {
				length++;
			}
		}
		charptrTS++;
	}
	

	/*Gets the last word*/
	TokenizerT temp = malloc(sizeof(struct TokenizerT_));
	char *String= malloc(sizeof(char)*(length+1));
	strncpy(String,begins,length);
	String[length]='\0';
	temp->token=String;	
	temp->next=NULL;
	if (rtrn==NULL){
		rtrn=temp;
	} else {
		rtrnPTR->next=temp;
	}
	rtrnPTR=temp;
	
	/*frees unnecessary mallocs*/
	free(sep);
	free(tokenString);
					
	return rtrn;
}

/*
 * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
 *
 * You need to fill in this function as part of your implementation.
 */

void TKDestroy(TokenizerT tk) {
	TokenizerT destroyPTR;				/*Pointer we want to destroy*/
	TokenizerT nextPTR;					/*Pointer to the next Token in object*/
	
	/*Frees all malloced space within TK and declares the items within tokenizerT as NULL*/
	destroyPTR=tk;
	nextPTR=tk;
	
	while(destroyPTR!=NULL){
		nextPTR=nextPTR->next;
		destroyPTR->token=NULL;
		destroyPTR->next=NULL;
		free(destroyPTR);
		destroyPTR=nextPTR;
	}
}

/*
 * TKGetNextToken returns the next token from the token stream as a
 * character string.  Space for the returned token should be dynamically
 * allocated.  The caller is responsible for freeing the space once it is
 * no longer needed.
 *
 * If the function succeeds, it returns a C string (delimited by '\0')
 * containing the token.  Else it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */

char *TKGetNextToken(TokenizerT tk) {
	/*NOTE FOR GRADER: I returned the token for the given TokenizerT. 
	 * I didn't see a use to return the next token, but I will add notes as to 
	 * what should be different to return the NEXT token string.*/
	 
	 /*if tk == NULL returns 0*/
	if (tk->token==NULL) {
		return "0"; 
	}
	 
	 /*returning String*/
	char *rtrn=NULL;
		rtrn=malloc(sizeof(char)*(strlen(tk->token)+1));
		/*(if returning next token the tk->token should be tk->next->token*/
		
	strcpy(rtrn,tk->token);
	/*if returning next token the tk->token should be tk->next->token*/
	
	return rtrn;
}

/*
 * main will have two string arguments (in argv[1] and argv[2]).
 * The first string contains the separator characters.
 * The second string contains the tokens.
 * Print out the tokens in the second string in left-to-right order.
 * Each token should be printed on a separate line.
 */
 
void printList(TokenizerT tk){
	TokenizerT printPTR=tk;			/*points to TokenizerT*/
	
	/*Prints the resulting list of TKCreate*/
	while (printPTR!=NULL) {
		if (strcmp(TKGetNextToken(printPTR),"0")!=0){
			printf("\t%s\n",TKGetNextToken(printPTR));
		}
		printPTR=printPTR->next;	
	}
}

