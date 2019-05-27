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
	int port, serverPort, sock, newsocket, n;
	socklen_t clientlen;
	struct sockaddr_in server, client;
	struct sockaddr *serverptr = (struct sockaddr *) &server;
	struct sockaddr *clientptr = (struct sockaddr *) &client;
	struct hostent *rem;
	

	if(argc<3){
		printf("PLease give port number!\n");
		exit(1);
	}
	strcpy(myIP, "123.456.789");
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


/*   printf("Type a message: ");
    fgets(buf,sizeof(buf),stdin);
	if('\n' == buf[strlen(buf) - 1])
    	buf[strlen(buf) - 1] = '\0';
*/

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
}


