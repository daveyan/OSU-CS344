#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/stat.h> 
#include <time.h>

//conveniently named rooms
char * roomName[10] = {
	"room1",
	"room2",
	"room3",
	"room4",
	"room5",
	"room6",
	"room7",
	"room8",
	"room9",
	"room10"
};
//room types
char * roomType[3] = {
	"START_ROOM",
	"MID_ROOM",
	"END_ROOM"
};

struct room {
	char * rName;		//room names
	char * rType;		//room types
	int nConnect;		//needed connections : generated from rand()
	int cConnect;		//current connections 
	int rConnect[6];	//int for easy access with roomName[] and struct room
	char * rInfo[6];	//room names stored as a string for convenience in the game
};

struct room rooms[7];
struct room gameRoom[1];

void shuffle(char * * array, size_t n) {
	if (n > 1) {
		size_t i;
		for (i = 0; i < n; i++) {
			size_t j = rand() % n;
			char * temp = array[j];
			array[j] = array[i];
			array[i] = temp;
		}
	}
}


void addConnections(struct room * rooms, int curRoom) {

	int newRoom, i;
	int exists;
	//while the number of connected rooms is fewer than the needed amount of rooms
	do {

		newRoom = rand() % 7;
		//check to see if the new room is the same as the current room
		if (rooms[curRoom].rName != rooms[newRoom].rName) {
			//check to see if it is already in the rConnect array
			exists = 0;
			for (i = 0; i < rooms[curRoom].cConnect; i++) {
				if (rooms[curRoom].rConnect[i] == newRoom) {
					exists = 1;
				}
			}
			//the connection does not exist in either room
			if (exists == 0) {

				rooms[curRoom].rConnect[rooms[curRoom].cConnect] = newRoom;
				rooms[curRoom].cConnect++;
				//newRoom needs to connect back to the original room
				rooms[newRoom].rConnect[rooms[newRoom].cConnect] = curRoom;
				rooms[newRoom].cConnect++;

			}

		}

	} while (rooms[curRoom].cConnect < rooms[curRoom].nConnect);

}

int createRooms(struct room * rooms) {

	//deciding which rooms will be which roomtype
	int randStartRoom, randEndRoom;
	int diffVal = 0;
	do {
		randStartRoom = rand() % 7;
		randEndRoom = rand() % 7;
		if (randStartRoom != randEndRoom) {
			diffVal = 1;
		}
	} while (diffVal = 0);

	//set the current number of connections to 0
	int j;
	for (j = 0; j < 7; j++) {
		rooms[j].cConnect = 0;
	}

	int i;
	for (i = 0; i < 7; i++) {
		//set the name of the room based on the randomized roomName arry
		rooms[i].rName = roomName[i];
		//set the room types
		if (i == randStartRoom) {
			rooms[i].rType = roomType[0];
		}
		else if (i == randEndRoom) {
			rooms[i].rType = roomType[2];
		}
		else {
			rooms[i].rType = roomType[1];
		}
		//set the number of connections
		rooms[i].nConnect = 3 + (rand() % 4);

		addConnections(rooms, i);

	}
	return randStartRoom;
}

//PURELY FOR DEBUGGING. NOT USED IN MAIN
void printRooms(struct room * rooms) {
	int i;
	printf("Name\tType\t\tNumCon\tConnections\n");
	for (i = 0; i < 7; i++) {

		printf("%s\t", rooms[i].rName);
		printf("%s\t", rooms[i].rType);
		printf("%d\t", rooms[i].nConnect);
		int j;
		for (j = 0; j < rooms[i].nConnect; j++) {
			printf("%s\t", rooms[rooms[i].rConnect[j]].rName);
		}
		printf("\n");
	}
}

void createFile(struct room * rooms) {

	//Making the dir string REF http://cboard.cprogramming.com/c-programming/165757-using-process-id-name-file-directory.html
	int buffer = 100;
	int pid = getpid();
	char * predir = "./yanda.rooms.";
	char * newDir = malloc(buffer);
	char * enddir = "/";
	sprintf(newDir, "%s%d%s", predir, pid, enddir);

	mkdir(newDir, 0775);

	int i;
	for (i = 0; i < 7; i++) {

		char newFile[100];
		strcpy(newFile, newDir);
		newFile[100] = 0;

		strcat(newFile, rooms[i].rName);

		FILE * fp;
		fp = fopen(newFile, "w");

		fprintf(fp, "Room Name: %s\n", rooms[i].rName);

		int j;
		for (j = 0; j < rooms[i].nConnect; j++) {
			fprintf(fp, "Connection %i: %s\n", j + 1, rooms[rooms[i].rConnect[j]].rName);
		}

		fprintf(fp, "Room Type: %s\n", rooms[i].rType);

		fclose(fp);
	}
}

void getFileData(struct room * gameRoom, int startR) {
	//same as create Files
	int buffer = 100;
	int pid = getpid();
	char * predir = "./yanda.rooms.";
	char * curDir = malloc(buffer);
	char * enddir = "/";
	sprintf(curDir, "%s%d%s%s", predir, pid, enddir, rooms[startR].rName);

	char textFile[1000];
	FILE * fp;

	fp = fopen(curDir, "r");
	if (fp == NULL) {
		printf("Could not open file");
	}
	gameRoom -> cConnect = 0;

	char text[100];
	fgets(text, 100, fp);
	//ROOM NAME
	char * name = strtok(text, " ");
	name = strtok(NULL, " ");
	name = strtok(NULL, "\n");

	//Storing the values from a token into a struct REF http://stackoverflow.com/questions/34956110/how-to-store-data-from-pointer-returned-by-strtok-into-struct-without-it-being-l	
	gameRoom -> rName = strdup(name);

	//CONNECTIONS
	int i;

	for (i = 0; i < rooms[startR].nConnect; i++) {
		fgets(text, 100, fp);
		char * con = strtok(text, " ");
		con = strtok(NULL, " ");
		con = strtok(NULL, "\n");

		char * conName = strdup(con);
	
		gameRoom -> rInfo[gameRoom -> cConnect] = conName;
		gameRoom -> cConnect++;

	}

	//ROOM TYPE
	fgets(text, 100, fp);
	char * type = strtok(text, " ");
	type = strtok(NULL, " ");
	type = strtok(NULL, "\n");

	gameRoom -> rType = strdup(type);

}

void main() {
	//seed for rand()
	srand(time(NULL));
	//shuffle the values in roomName[]
	shuffle(roomName, 10);

	int startR;			//value to be passed to the getFileData as the start room of the game

	//create rooms assigns the name, connections, and type
	startR = createRooms(rooms);

	//FOR DEBUGGING ONLY
//	printRooms(rooms);

	int numOfSteps;
	numOfSteps = 0;
	int endRoom;
	endRoom = 0;

	char playerMove[100];
	char * playerPath[100];
	
	//create the files int he directory
	createFile(rooms);
	int i;

	getFileData(gameRoom, startR);
	do {
		//check to see if your are in the end room
		playerPath[numOfSteps] = gameRoom -> rName;
		if (strcmp(gameRoom -> rType, roomType[2]) == 0) {
			printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
			printf("YOU TOOK %i STEPS. YOUR PATH TO VICTORY WAS:\n", numOfSteps);
			for (i = 0; i < numOfSteps; i++) {
				printf("%s\n", playerPath[i]);
			}
			printf("%s\n", gameRoom -> rName);

			endRoom = 1;
		}
		else {
			printf("CURRENT LOCATION: %s\n", gameRoom -> rName);
			printf("POSSIBLE CONNECTIONS: ");
			int i, j;
			int exists;
			exists = 0;
			for (i = 0; i < gameRoom -> cConnect; i++) {
				printf("%s  ", gameRoom -> rInfo[i]);
			}
			//user submits a new room
			printf("\nWHERE TO? >");
			scanf("%99s", playerMove);

			//check to see if the room is in list of rInfo for the current room
			for (j = 0; j < gameRoom -> cConnect; j++) {
				if (strcmp(playerMove, gameRoom -> rInfo[j]) == 0) {
					exists = 1;
				}
			}

			//if room exists in rInfo

			if (exists) {
				int i;
				for (i = 0; i < 7; i++) {
					if (strcmp(playerMove, rooms[i].rName) == 0) {
						numOfSteps++;
						getFileData(gameRoom, i);
					}
				}

			}
			else {
				printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
			}
		}
	} while (!endRoom);

	getchar();
}
