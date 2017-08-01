#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define SIZE 2048

int main (int argc, char* argv[]){

	int portnum, sockfd;
	struct sockaddr_in server;
	char text[SIZE];

	//check usage
	if(argc < 4){
		fprintf(stderr,"USAGE: %s text key port\n",argv[0]);
		exit(1);
	}

	//convert string port to int
	portnum = atoi(argv[3]);

	//open socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		fprintf(stderr,"Open Error\n");
		exit(1);
	}
	//set up server
	memset(&server,'\0',sizeof(server));

	server.sin_family = AF_INET;
	server.sin_port = htons(portnum);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	//connect to a socket
	if(connect(sockfd,(struct sockaddr*)&server, sizeof(server)) < 0){
		fprintf(stderr,"Connect Error 1\n");
		exit(2);
	}


	//writing the file
	int writeCheck;
	snprintf(text,SIZE,"%s",argv[1]);
	writeCheck = write(sockfd,text, strlen(text));
	if(writeCheck < 0){
		fprintf(stderr,"Write Error\n");
		exit(1);
	}

	sleep(1);

	//writing the key
	snprintf(text,SIZE,"%s",argv[2]);
	writeCheck = write(sockfd,text,strlen(text));
	if(writeCheck < 0){
		fprintf(stderr,"Write Error\n");
		exit(1);
	}
	
	memset(&text,'\0',sizeof(text));
	//get response
	int readCheck;
	readCheck = read(sockfd, text, SIZE);
	if(readCheck < 0){
		fprintf(stderr,"Connect Error 2\n");
		exit(2);
	}

	if(strcmp(text,"short") == 0){
		close(STDOUT_FILENO);

		fprintf(stderr,"Key too short\n");
		exit(1);
	}
	else if(strcmp(text,"bad") == 0){
		close(STDOUT_FILENO);
		
		fprintf(stderr,"Bad characters\n");
		exit(1);
	}
	else{
		printf("%s\n",text);

	}
	close(sockfd);
	

return 0;
}
