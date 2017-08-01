#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_ARG 512 
#define MAX_BUFF 2048

int argCnt = 0;			//argument count from strtok of userInput
int background = 0;		//whether or not a process is in the background


//FOR DEBUGGING PURPOSES
/*
void  printArgs(char* userInput, char** userArgs){
	if(argCnt > 0){
		int i;
		printf("[%d]",argCnt);
		for (i = 0; i < argCnt; i++){
			printf("[Argument %d]: %s \t",i+1,userArgs[i]);
		}
		printf("\n");
	}
}
*/


//"Establishing a Signal Handler" REF - http://pubs.opengroup.org/onlinepubs/009695399/functions/sigaction.html
void sig_handler(int sig){
	printf("terminated by signal %d\n",sig);
	
}


//Handles all non cd status and exit commands
int bashCmd(char **userArgs){

	pid_t pid, wpid;
	int status = 0;
	int exitStatus = 0;

	//man page sigaction REF - http://man7.org/linux/man-pages/man2/rt_sigaction.2.html
	//Also refer to "Establishing a Signal Handler"
	struct sigaction si; 
	si.sa_handler = sig_handler;
	// handles a signal interrupt
	sigaction(SIGINT, &si, NULL);

	//fork to create child
	pid = fork();

	//CHILD PROCESS
	if(pid == 0){
		if(execvp(userArgs[0],userArgs) == -1){
			//perror("execvp");
			printf("no such file or directory\n");
			exitStatus = 1;
			exit(1);
		}
	}
	//FORK ERR
	else if(pid < 0){
		perror("smallsh");
		
	}
	//PARENT PROCESS
	else {
		//waiting for a signal to see if the process finishes normally or receives a signal
		do{
			//waiting for the child process to complete or any child process
			if(background == 0){
				//if child stopped
				wpid = waitpid(pid,&status,WUNTRACED);
			}
			else if(background == 1){
				wpid = waitpid(-1, &status,WNOHANG);
			}
		}while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}



	//background process is complete	
	if(background == 1) {
		printf("background pid %d exit value %d\n", pid,exitStatus);
	}
	//if something was not caught
	if(status != 0 || WIFSIGNALED(status)){
		exitStatus = 1;
	}

	return exitStatus;

}



int main(){

	int shell = 0;
	int exitValue = 0;

	//loop til exit is called (!shell)
	do{
		char *userInput = NULL;
		size_t size = MAX_BUFF;
		char **userArgs;
		int fDesc;
		argCnt = 0;
		background = 0;
		


		printf(": ");
		//reading the users cmd line
		getline(&userInput,&size, stdin);
		//tokenizing the users Input
		int argSize = MAX_ARG;
		int tokCnt = 0;
		userArgs = malloc(sizeof(char*)* argSize);
		char *tok;
		
		tok = strtok(userInput, " \n");

		while (tok !=NULL){
			userArgs[tokCnt] = tok;
			tokCnt++;
			argCnt++;

			tok = strtok(NULL, " \n");
		}
		userArgs[tokCnt] = NULL;

		//check to see if there are arguments
		if(argCnt > 0){

			//DEBUG TESTER
			//printArgs(userInput, userArgs);
		
			if(strncmp(userArgs[argCnt-1],"&",1) == 0){
				//printf("background");
				userArgs[argCnt-1] = NULL;
				argCnt--;
				//this is now a background process
				background = 1;
			}
	

			//BUILT IN COMMAND [EXIT]
			if(strcmp(userArgs[0],"exit") == 0){
				shell = 1;
			}
		
			//BUILT IN COMMAND [CD]
			else if(strcmp(userArgs[0],"cd") == 0){
				//check to see if there is a dir in the arguments
				if(userArgs[1] == NULL){
					chdir(getenv("HOME"));
					exitValue = 0;
				}
				else if(chdir(userArgs[1]) != 0) {
					printf("No such file or directory\n");
					exitValue = 1;
				}
				else{
					char* home;
					home = getenv("HOME");
					char* newDir;
					newDir = ("%s/%s",home,userArgs[1]);
					chdir(newDir);
					exitValue = 0;
				}
			}
			//BUILT IN COMMAND [STATUS]
			else if(strcmp(userArgs[0],"status") == 0){
				printf("exit value %d\n",exitValue);
				exitValue = 0;
				
			}

			//HANDLING COMMENTS [#]
			else if(strncmp(userArgs[0],"#",1) == 0){
				//printf("Comment: doing nothing\n");
				exitValue = 0;
			}


		
			//CHECKING FOR FILE REDIRECTION 
			else if(argCnt == 3 && (strcmp(userArgs[1],">") == 0 ||strcmp(userArgs[1],"<") == 0)){
				//WRITE	[>]- check to see if there is an output redirection char and a third arugment	
				if(strcmp(userArgs[1],">") == 0 && userArgs[2] != NULL){
					//printf("File Write\n");

					int in, out;
					in = dup(0);	//create a copy of the file desc
					out = dup(1);	//create a copy of the file desc
				

					fDesc = open(userArgs[2], O_WRONLY | O_TRUNC|O_CREAT, 0644);
					//check if we were able to open the file
					if(fDesc == -1){
						printf("cannot open %s for output\n",userArgs[2]);
						exitValue = 1;
					}				
					else{
						dup2(fDesc,1); 		//change to stdout
						userArgs[1] = NULL;	//consider only the first cmd
						close(fDesc);
						exitValue = bashCmd(userArgs); //execute in bash
					}
					dup2(in,0); 	//restore to the original 
					close(in);
					dup2(out,1);	//restore to the original
					close(out);
				}
	
				//READ	[<]- check to see if there is an output redirection char and a third arugment	
				else if(strcmp(userArgs[1],"<") == 0 && userArgs[2] != NULL){
					//printf("File Read\n");
					
					int in, out;

					in = dup(0);
					out = dup(1);
					
					fDesc = open(userArgs[2], O_RDONLY);
					
					if(fDesc == -1){
						printf("cannot open %s for input\n",userArgs[2]);
						exitValue = 1;
					}
					else{
						dup2(fDesc, 0);
						userArgs[1] = NULL;
						close(fDesc);
						exitValue = bashCmd(userArgs);
					}

					dup2(in, 0);
					close(in);
					dup2(out,1);
					close(out);
				}
			}
			else{
				exitValue = bashCmd(userArgs);
			}
			
			free(userArgs);
		}

		else{
			//printf("Empty line: doing nothing\n");
		}
	}while(!shell);

return 0;
}





