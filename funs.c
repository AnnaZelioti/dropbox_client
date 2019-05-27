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
void insertList(listptr* mylist, char* name) {
    listptr temp;
    temp=*mylist;

    *mylist=malloc(sizeof(struct listnode));
	if(mylist==NULL){
		printf ("Could not allocate memory.\n");
		exit(1);
	}

    (*mylist)->next=temp;
	(*mylist)->filename=malloc(strlen(name) + 1);
	strcpy((*mylist)->filename,name);

}

void destroyList(listptr *mylist){
	listptr temp;

	while((*mylist)!=NULL){
		temp=*mylist;
		(*mylist)=(*mylist)->next;
		free(temp->filename);
		free(temp);
	}
}

int isInList(listptr mylist, char* name){

	while(mylist!=NULL){
		if(strcmp(mylist->filename,name)==0){
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
	    printf("%s--> ", list->filename);      
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



