#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define SIZE 2048

int isChar(char c){
	switch(c){
		case 'A': return 0;
		case 'B': return 0;
		case 'C': return 0;
		case 'D': return 0;
		case 'E': return 0;
		case 'F': return 0;
		case 'G': return 0;
		case 'H': return 0;
		case 'I': return 0;
		case 'J': return 0;
		case 'K': return 0;
		case 'L': return 0;
		case 'M': return 0;
		case 'N': return 0;
		case 'O': return 0;
		case 'P': return 0;
		case 'Q': return 0;
		case 'R': return 0;
		case 'S': return 0;
		case 'T': return 0;
		case 'U': return 0;
		case 'V': return 0;
		case 'W': return 0;
		case 'X': return 0;
		case 'Y': return 0;
		case 'Z': return 0;
		case ' ': return 0;
		default: return 1;
	}
}
//decrypt
int deChar(char c){
	switch(c){
		case 'A': return 0;
		case 'B': return 1;
		case 'C': return 2;
		case 'D': return 3;
		case 'E': return 4;
		case 'F': return 5;
		case 'G': return 6;
		case 'H': return 7;
		case 'I': return 8;
		case 'J': return 9;
		case 'K': return 10;
		case 'L': return 11;
		case 'M': return 12;
		case 'N': return 13;
		case 'O': return 14;
		case 'P': return 15;
		case 'Q': return 16;
		case 'R': return 17;
		case 'S': return 18;
		case 'T': return 19;
		case 'U': return 20;
		case 'V': return 21;
		case 'W': return 22;
		case 'X': return 23;
		case 'Y': return 24;
		case 'Z': return 25;
		case ' ': return 26;
	}
}


int main (int argc, char* argv[]){
	
	int portnum, sockfd, sockconnect;
	pid_t pid,sid;
	struct sockaddr_in server, client;
	socklen_t clientlen;
	char in[SIZE];		//receiving array
	char out[SIZE];		//sending array
	char cipher[SIZE];
	char key[SIZE];
	char rtext[SIZE]; 	//return text
	char letters[28] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

	//checking correct usage
	if(argc < 2){
		fprintf(stderr,"USAGE: %s listening_port",argv[0]);
		exit(1);
	}
	
	//convert the string port to int 
	portnum = atoi(argv[1]);
	
	//create a child
	pid = fork();
	if(pid < 0){ 
		exit(1);
	}
	if(pid > 0){ 
		exit(0);
	}
	//file permissiongs
	umask(0);

	//child process
	sid = setsid();
	if(sid < 0){
		exit(1);
	}

	close(STDOUT_FILENO);
	close(STDIN_FILENO);
	
	//open the socket for the port
	sockfd = socket(AF_INET, SOCK_STREAM,0);
	if(sockfd < 0){
		//socket open err
		fprintf(stderr,"Socket Error\n");
		exit(1);
	}
	
	//set up the server
	memset(&server,'\0',sizeof(server));

	server.sin_family = AF_INET;
	server.sin_port = htons(portnum);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind socket
	if(bind(sockfd,(struct sockaddr*)&server, sizeof(server))< 0){
		fprintf(stderr,"Bind Error\n");
		exit(1);
	}

	//per specification allow up to 5 connections
	listen(sockfd, 5);
	
	clientlen = sizeof(client);
	//continue to listen unless broken out of
	while(1){
		sockconnect = accept(sockfd,(struct sockaddr*)&client, &clientlen);
		//unable to connect to socket
		if(sockconnect < 0){
			fprintf(stderr,"Accept Error");
			exit(1);
		}

		int readCheck;
		//read cipher
		
		memset(&in,'\0',sizeof(in));

		readCheck = read(sockconnect, in, SIZE);
		if(readCheck < 0){
			fprintf(stderr,"Read Error\n");
			exit(1);
		}

		FILE *fp1;
		fp1 = fopen(in,"r");
		if (fp1 == NULL){
			fprintf(stderr,"Open Error\n");
			exit(1);
		}
		
		fgets(cipher,SIZE,fp1);
		int i;
		//change all of the cipher char toupper
		for(i = 0; i < strlen(cipher); i++){
			cipher[i] = toupper(cipher[i]);
		}
		//check for invalid
		for(i = 0; i < strlen(cipher); i++){
			if(isChar(cipher[i]) == 1){
				fprintf(stderr,"Input contains bad char Error\n");
				exit(1);
			}
		}
		fclose(fp1);

		//read key
		memset(&in,'\0',sizeof(in));
		readCheck = read(sockconnect, in, SIZE);
		if(readCheck < 0){
			fprintf(stderr,"Read Error\n");
			exit(1);
		}

		FILE *fp2;
		fp2 = fopen(in,"r");
		if(fp2 == NULL){
			fprintf(stderr,"Open Error\n");
			exit(1);
		}

		fgets(key,SIZE,fp2);
		
		fclose(fp2);

		//need to check if the key is as long as the cipher
		if(strlen(cipher) > strlen(key)){
			fprintf(stderr,"Key too short Error\n");
			exit(1);
		}
		
		//number arrays for decryption
		int rtextConvert[strlen(cipher)], cipherConvert[strlen(cipher)], keyConvert[strlen(cipher)];

		int j;
		//change all of the char into numbers
		//mod conversion and then return them back into char
		for(j = 0; j < strlen(cipher); j++){
			cipherConvert[i] = deChar(cipher[i]);
			keyConvert[i] = deChar(key[i]);
			rtextConvert[i] = cipherConvert[i] - keyConvert[i];

			if(rtextConvert[i] < 0){
				rtextConvert[i] = rtextConvert[i] + 27;
			}
			rtext[i] = letters[rtextConvert[i]];
		}

		memset(&out,'\0',sizeof(out));

		snprintf(out, SIZE,"%s",rtext);
		int writeCheck;
		writeCheck = write(sockconnect,out, strlen(out));
		if(writeCheck < 0){
			fprintf(stderr,"Write Error\n");
			exit(1);
		}

	}
	

return 0;
}
















