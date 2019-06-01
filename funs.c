#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "header.h"
#include <string.h>



//List functions
void insertList(listptr* mylist, char* IP, int port) {
    listptr temp;
    temp=*mylist;

    *mylist=malloc(sizeof(struct listnode));
	if(mylist==NULL){
		printf ("Could not allocate memory.\n");
		exit(1);
	}

    (*mylist)->next=temp;
	(*mylist)->clientIP=malloc(strlen(IP) + 1);
	strcpy((*mylist)->clientIP,IP);
	(*mylist)->clientPort=port;

}
int deleteClient(listptr *mylist, char* IP, int port){ 
	listptr templist;
  	while ((*mylist) != NULL) {  
    	if((strcmp((*mylist)->clientIP,IP)==0)&&((*mylist)->clientPort==port)){
      		templist = *mylist;         
      		*mylist = (*mylist)->next;
			free(templist->clientIP);	       
      		free(templist);    
      		return 1;                          
    	}
    	else
     	mylist = &((*mylist)->next);
}
  	return 0;        
}

void destroyList(listptr *mylist){
	listptr temp;

	while((*mylist)!=NULL){
		temp=*mylist;
		(*mylist)=(*mylist)->next;
		free(temp->clientIP);
		free(temp);
	}
}

int isInList(listptr mylist, char* IP, int port){

	while(mylist!=NULL){
		if((strcmp(mylist->clientIP,IP)==0)&&(mylist->clientPort==port)){
			printf("Found it.\n");
			return 1;
		}
		else
			mylist=mylist->next;
	}
	return 0;

}

void print(listptr list){ 
	while (list != NULL) {      
	    printf("IP %s, port %d --> ", list->clientIP, list->clientPort);      
		list = list->next;               
	}
	printf("NULL\n"); 
}
	

//Other functions
int numOfDigits(int num){
	int count=0;
	while(num!=0){
		num= num/10;
		count++;
	}
	return count;
}

void perror_exit(char* message){
    perror(message);
    exit(EXIT_FAILURE);
}



