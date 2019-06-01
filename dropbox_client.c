#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "header.h"


int main(int argc, char* argv[]){

	char serverIP[10], buf[1024], myIP[12], command[14], recip[14], recClientPort[10], *ip;
	long int lip;
	int port, iport, serverPort, sock, newsocket, mysock, n, m, receiveLen, c, clientPort;
	socklen_t clientlen;
	struct sockaddr_in server, client, myserver, dummy;
	struct sockaddr *serverptr = (struct sockaddr *) &server;
	struct sockaddr *clientptr = (struct sockaddr *) &client;
	struct sockaddr *myserverptr= (struct sockaddr *) &myserver;
	struct hostent *rem;
	listptr clientList=NULL;
	

	if(argc<6){
		printf("Please give all inputs!\n");
		printf("Example: ./server -p portNum -sp serverPort -sip serverIP\n");
		exit(1);
	}
	strcpy(myIP, "127.0.0.1");
	strcpy(serverIP,argv[6]);
	port=atoi(argv[2]);
	serverPort=atoi(argv[4]);

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
	n=numOfDigits(htons(port));
	m=numOfDigits(dummy.sin_addr.s_addr);
	snprintf(buf, 9 + strlen(myIP) + n +m, "LOG_ON %u %d" , dummy.sin_addr.s_addr, htons(port) );
    //Send the character
    if(send(sock,buf,strlen(buf),0)==-1)
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
	if(send(sock, "GET_CLIENTS", strlen("GET_CLIENTS"),0)==-1)
		perror_exit("write");

	while(1){
		//Receive client list
		receiveLen = recv(sock, buf, 1024 ,0);
    	if (receiveLen == -1){
        	printf("Error: receive has been  %d\n",sock);
        	close(sock);
			perror_exit("receive");
    	}
		//Insert received client to list 
		lip=atol(recip);  // Convert to long int 
		dummy.sin_addr.s_addr=lip;
		ip=inet_ntoa(dummy.sin_addr); // convert to string format ex 123.123.123.123)
		iport=atoi(recClientPort);
		clientPort=ntohs(iport);
		insertList(&clientList, ip, clientPort);
		printf("I received %s\n", buf);
		break;
	}
	//Close socket 
	close(sock);

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

		sscanf(buf,"%s %s %s", command, recip, recClientPort);
		printf("Received command: <%s>\n",command);
		lip=atol(recip);  // Convert to long int
		ip=inet_ntoa(dummy.sin_addr); // convert to string format ex 123.123.123.123) 
		dummy.sin_addr.s_addr=lip;

		iport=atoi(recClientPort);
		clientPort=ntohs(iport);

		if( strcmp(command, "GET_FILE_LIST")==0) 
			c=1;
		if(strcmp(command, "GET_FILE")==0) 
			c=2;
		if(strcmp(command, "USER_OFF")==0)
			c=3;
		if(strcmp(command, "USER_ON")==0)
			c=4;

		switch(c){
			case 1:
				//GET_FILE_LIST
				break;
			case 2:
				//GET_FILE
				break;
			case 3:
				//USER_OFF
				break;
			case 4:
				//USER_ON
				printf("IP: %s, port: %d\n", ip, clientPort);
				
				break;
			default:
				printf("Unknown command. Doing nothing!\n");
				break;
		}

		close(newsocket);
		break;
	}
	close(sock);
	return 0;

}

