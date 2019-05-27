//header.h

typedef struct listnode * listptr;

struct listnode {

	char* filename;
	int id;
	listptr next;

};


//List funcions
void insertList(listptr*, char*);
void destroyList(listptr*);
int isInList(listptr,char*);
void print(listptr);


//Other 
int numOfDigits(int);
void perror_exit(char* message);
int numOfDigits(int num);
