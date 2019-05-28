#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include "header.h"


int main(int argc, char* argv[]){

	char serverIP[10], buf[1024], myIP[12];
	int port, serverPort, sock, newsocket, mysock, n, receiveLen;
	socklen_t clientlen;
	struct sockaddr_in server, client, myserver;
	struct sockaddr *serverptr = (struct sockaddr *) &server;
	struct sockaddr *clientptr = (struct sockaddr *) &client;
	struct sockaddr *myserverptr= (struct sockaddr *) &myserver;
	struct hostent *rem;
	

	if(argc<3){
		printf("Please give port number!\n");
		exit(1);
	}
	strcpy(myIP, "127.0.0.1");
	strcpy(serverIP,argv[6]);
	port=atoi(argv[2]);
	serverPort=atoi(argv[4]);

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
	n=numOfDigits(port);
	snprintf(buf, 9 + strlen(myIP) + n , "LOG_ON %s %d" , myIP, port );
    //Send the character
    if(send(sock,buf,strlen(buf),0)==-1)
		perror_exit("write");
	
    //Recieve the character
//	if(recv(sock, buf,strlen(buf), 0)==-1)
//	    perror_exit("read");
    
//    printf("Recieved sting: %s",buf);



	//Close socket and exit
	close(sock);

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

	while(1){
   		clientlen=sizeof(client);

 		//Accept Connection
    	if((newsocket=accept(mysock, clientptr, &clientlen))<0)
			perror_exit("accept");
   
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
		close(newsocket);
		break;
	}
	close(sock);
	return 0;

}

