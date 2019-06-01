//header.h

typedef struct listnode * listptr;

struct listnode {

	char* clientIP;
	int clientPort;
	listptr next;

};


//List funcions
void insertList(listptr*, char*, int);
void destroyList(listptr*);
int deleteClient(listptr*, char*, int);
int isInList(listptr,char*, int);
void print(listptr);


//Other 
int numOfDigits(int);
void perror_exit(char* message);
int numOfDigits(int num);
