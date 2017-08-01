#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define SIZE 2048


int main (int argc, char* argv[]){

	int portnum,sockfd;
	struct sockaddr_in server;
	char text[SIZE];

	//check usage
	if(argc < 4){
		fprintf(stderr,"USAGE: %s cipher key port\n",argv[0]);
		exit(1);
	}
	//convert string port to an int
	portnum = atoi(argv[3]);

	//open socket
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		fprintf(stderr,"Open Error\n");
		exit(1);
	}

	//setup server
	memset(&server,'\0',sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(portnum);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	//connect
	if(connect(sockfd,(struct sockaddr*)&server, sizeof(server)) < 0){
		fprintf(stderr,"Connect Error\n");
		exit(2);
	}

	//write cipher to socket
	snprintf(text,SIZE,"%s",argv[1]);
	int writeCheck;
	writeCheck = write(sockfd, text,strlen(text));
	if(writeCheck < 0){
		fprintf(stderr,"Write Error\n");
		exit(1);
	}

	sleep(1);

	//write key to socket
	snprintf(text, SIZE,"%s",argv[2]);
	writeCheck = write(sockfd,text,strlen(text));
	if(writeCheck < 0){
		fprintf(stderr,"Write Error\n");
		exit(1);
	}
	//connect to server
	memset(&text,'\0',sizeof(text));

	int readCheck;
	readCheck = read(sockfd,text,SIZE);
	if(readCheck < 0){
		fprintf(stderr,"Connect Error\n");
		exit(2);
	}

	printf("%s\n",text);
	close(sockfd);

return 0;
}

