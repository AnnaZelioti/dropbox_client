#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "header.h"
#include <signal.h>
#include <fcntl.h>

pthread_mutex_t accept_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;
listptr clientList=NULL;
listptr fileList=NULL;
int bufSize;


void signal_handler(int signo){
	if(signo==SIGINT)
		printf("I received sigint\n");
	//run=0;
	return;
} 

void listen_for_conns(void *listenSocket) {
	struct sockaddr_in client, dummy;
	struct sockaddr *clientptr = (struct sockaddr *) &client;
	char buf[1024], command[14], frec[100], srec[10];
	long int lip;
	int iport, newsocket, receiveLen, c, clientPort, fd, remaining, fileSize, readSize;
	struct hostent *rem;
	socklen_t clientlen;
	listptr templist;
	struct stat st;
//	struct sigaction sa;
//	sa.sa_handler=signal_handler;

	while(1){
//		if(sigaction(SIGINT, &sa,NULL)==-1)
//			perror("SIGACTION");
   		clientlen=sizeof(client);
		printf("Listening for connections on port %d\n", *(int*)listenSocket);
 		//Accept Connection
        pthread_mutex_lock(&accept_mutex);
    	if((newsocket=accept(*(int*)listenSocket, clientptr, &clientlen))<0)
			perror_exit("accept");
        pthread_mutex_unlock(&accept_mutex);
   
    	//Find client's name
    	if((rem=gethostbyaddr((char*)&client.sin_addr.s_addr,sizeof(client.sin_addr.s_addr),client.sin_family))==NULL){
			herror("gethostbyaddr");
			exit(1);
    	}
    	printf("Accepted connection from %s\n", rem->h_name);

		receiveLen = recv(newsocket, buf, 1024 ,0);
    	if (receiveLen == -1){
        	printf("Error: receive has been  %d\n", newsocket);
        	close(newsocket);
        	exit(1);
    	}
		printf("I received %s\n", buf);

		sscanf(buf,"%s %s %s", command, frec, srec);
		printf("Received command: <%s>\n",command);

		if( strcmp(command, "GET_FILE_LIST")==0) 
			c=1;
		if(strcmp(command, "GET_FILE")==0) 
			c=2;
		if(strcmp(command, "USER_OFF")==0){
			lip=atol(frec);  // Convert to long int
			iport=atoi(srec);
			clientPort=ntohs(iport);
			c=3;
		}
		if(strcmp(command, "USER_ON")==0){
			lip=atol(frec);  // Convert to long int
			iport=atoi(srec);
			clientPort=ntohs(iport);
			c=4;
		}	
		switch(c){
			case 1:
				//GET_FILE_LIST request
				//Send the list with file names to the client that requested it 
				templist=fileList;
				while(templist!=NULL){
					send(newsocket,templist->clientIP, strlen(templist->clientIP) +1,0); //clientIP==fileName (same struct)
					templist=templist->next;
				}
				break;
			case 2:
				//GET_FILE request
			
				templist=fileList;
				while(templist!=NULL){
					if(isInList(templist,frec,0)){ //Send the file if we have it 
						//Get the file Size
						stat(templist->clientIP, &st);
						fileSize = st.st_size;
						if((fd = open(templist->clientIP, O_RDONLY)) < 0 ) {
							printf("Error in sendFile : open\n");
							exit(-1);
						}
						remaining=fileSize;
						while (remaining > 0) {
							readSize = (remaining > bufSize) ? bufSize : remaining;
							if(read(fd,buf,readSize) == -1) {
								printf("Error in writing file: read from file\n");
								exit(-1);
							}
							send(newsocket,buf,readSize,0);
							remaining = remaining - readSize;
							memset(buf, '\0', bufSize);
						}
						close(fd);
						break;
					}
					templist=templist->next;
				}
				break;
			case 3:
				//USER_OFF
                dummy.sin_addr.s_addr = lip;
                pthread_mutex_lock(&list_mutex);
                deleteClient(&clientList, inet_ntoa(dummy.sin_addr), clientPort);
                pthread_mutex_unlock(&list_mutex);
                printf("USER_OFF: IP: %s, port: %d\n", inet_ntoa(dummy.sin_addr), clientPort);
				break;
			case 4:
				//USER_ON
                dummy.sin_addr.s_addr = lip;
                pthread_mutex_lock(&list_mutex);
                insertList(&clientList, inet_ntoa(dummy.sin_addr), clientPort);
				print(clientList);
                pthread_mutex_unlock(&list_mutex);
                printf("USER_ON: IP: %s, port: %d\n", inet_ntoa(dummy.sin_addr), clientPort);
				break;
			default:
				printf("Unknown command. Doing nothing!\n");
				break;
		}

		close(newsocket);
		break;
	}
}


int main(int argc, char* argv[]){

	char serverIP[10], buf[1024], myIP[12], command[14], *directory;
	int port, serverPort, sock, mysock, receiveLen, numClients, clientPort, workerThreads;

	struct sockaddr_in server,myserver, dummy;
	struct sockaddr *serverptr = (struct sockaddr *) &server;
	struct sockaddr *myserverptr= (struct sockaddr *) &myserver;
	struct hostent *rem;
	

	if(argc<13){
		printf("Please give all inputs!\n");
		printf("Example: ./client -p portNum -w workerThreads -sp serverPort -sip serverIP\n");
		exit(1);
	}

	for(int i=1; i<argc; i++){
    	if(argv[i]!=NULL){
			//Directory path
			if(!strcmp(argv[i], "-d")){
				directory=malloc(strlen(argv[i+1]) +1);
				strcpy(directory,argv[i+1]);			
			}
			//PortNum
			else if(!strcmp(argv[i], "-p")){
				port=atoi(argv[i+1]);
			}
			//Nunmber or threads
			else if(!strcmp(argv[i], "-w")){
				workerThreads=atoi(argv[i+1]);
			}
			//Buffer size
			else if(!strcmp(argv[i], "-b")){
				bufSize=atoi(argv[i+1]);
			}
			//Server port
			else if(!strcmp(argv[i], "-sp")){
				serverPort=atoi(argv[i+1]);
			}
			//Server IP
			else if(!strcmp(argv[i], "-sip")){
				strcpy(serverIP, argv[i+1]);
			}	
		}
	}

	strcpy(myIP, "127.0.0.1");
	listFiles(directory, &fileList);

	//Set up  and wait for connections from other clients or the server
 
	//Create socket 
	if((mysock=socket(AF_INET,SOCK_STREAM,0))<0)
   		perror_exit("socket");

	myserver.sin_family=AF_INET; //internet domain
	myserver.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
	myserver.sin_port=htons(port); //given port

	//Bind socket to addr
	if((bind(mysock,myserverptr,sizeof(myserver)))<0)
    	perror_exit("bind");

	//Listen for connections
	if(listen(mysock,5)<0)
  	  perror_exit("listen");
	printf("Listening for connections to port %d\n",port);

    for (int i = 0; i < workerThreads-1; i++) {
        pthread_t thr;
        pthread_create(&thr, NULL, listen_for_conns, (void*)&mysock);
    }

	//Connect to server

	//Create socket
	if((sock=socket(AF_INET,SOCK_STREAM,0))<0)
 	   perror_exit("socket");

	//Find server address 
	if((rem=gethostbyname(serverIP))==NULL){
		herror("gethostbyname");
    	exit(1);
	}

	server.sin_family=AF_INET;
	memcpy(&server.sin_addr,rem->h_addr, rem->h_length);
	server.sin_port=htons(serverPort);

	//Initiate Connection
	if((connect(sock,serverptr,sizeof(server)))<0)
 	   perror_exit("connect");
	printf("Connecting to %s serverPort %d\n",argv[4],serverPort);

	//message="LOG_ON"
	inet_aton(myIP,&dummy.sin_addr);
	snprintf(buf, sizeof(buf), "LOG_ON %u %d" , dummy.sin_addr.s_addr, htons(port) );
    //Send the character
    if(send(sock,buf,strlen(buf)+1,0)==-1)
		perror_exit("write");
	close(sock);

	//new connection for getclients request
	if((sock=socket(AF_INET,SOCK_STREAM,0))<0)
 	   perror_exit("socket");
	//Initiate Connection
	if((connect(sock,serverptr,sizeof(server)))<0)
 	   perror_exit("connect");
	printf("Connecting to %s serverPort %d\n",argv[4],serverPort);

	//Send the GET_CLIENTS request 
	snprintf(buf, sizeof(buf), "GET_CLIENTS %u %d" , dummy.sin_addr.s_addr, htons(port) );
	if(send(sock, buf, strlen(buf)+1,0)==-1)
		perror_exit("write");

    receiveLen = recv(sock, buf, 17 ,0); // get CLIENT_LIST 000x
	if(receiveLen<0)
		perror_exit("receive");
    printf("Received CL: %s\n", buf);
    sscanf(buf,"%s %d", command, &numClients);
	for (int i = 0; i < numClients; i++){
		//Receive client list
        recv(sock, &dummy.sin_addr.s_addr, sizeof(dummy.sin_addr.s_addr), 0);
        recv(sock, &clientPort, sizeof(clientPort), 0);
        dummy.sin_port = htons(clientPort);
        printf("Got client %s:%d\n", inet_ntoa(dummy.sin_addr), ntohs(clientPort));
        pthread_mutex_lock(&list_mutex);
		insertList(&clientList, inet_ntoa(dummy.sin_addr), clientPort);
        pthread_mutex_unlock(&list_mutex);
	}
	//Close socket 
	close(sock);

    listen_for_conns((void*)&mysock);

	return 0;

}

