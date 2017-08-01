#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]){

	//seeding rand
	srand(time(NULL));
	
	//check that we have enough arguments
	if(argc != 2){
		printf("Usage: keygen keylength");
		exit(1);
	}

	char alphabet[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	int i;
	int rCharIdx;
	for (i = 0; i < atoi(argv[1]); i++){
		rCharIdx = rand() % 27;
		printf ("%c",alphabet[rCharIdx]);
	}
	//last char will be a new line
	printf("\n"); 

return 0;
}
