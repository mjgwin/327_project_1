#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <endian.h>
#include <unistd.h>
#include "project1-02.h"


int main(int argc, char *argv[]){

  srand(time(0));
  char *home = getenv("HOME");
  char *gameDir = ".rlg327";
  char *saveFile = "dungeon";
  //3 is for 2 slashes + null terminator
  char *filePath = malloc(strlen(home) + strlen(gameDir) + strlen(saveFile) + 3);
  sprintf(filePath, "%s/%s/%s", home, gameDir, saveFile);
  //printf("%s\n", filePath);
  
  if(argc == 1) generate_random();
  else{
    int i;
    for(i = 1; i < argc; i++) {
      if(!strcmp("--load", argv[i])) LOAD = 1;
      else if(!strcmp("--save", argv[i])) SAVE = 1;
      else{ 
        printf("Usage: --save | --load\n");
        return 0;
      }
    }
    
    if(LOAD) {
      if(access(filePath, F_OK) == 0) {
        load_file(filePath);
      }
      else{
        generate_random();
      }
    }
    else generate_random(filePath);
    
    if(SAVE) {
      save_file(filePath);
    }
  }
  
  print_world(d.world);
  clean_up();
  return 0;
  
}


void save_file(char *filePath) {
  FILE *f;
  //printf("made it to save_file");
  if(!(f = fopen(filePath, "w"))) {
    fprintf(stderr, "Failed to open file for writing");
    
    return;
  }
  
  char *name = "RLG327-S2021";
  fwrite(name, sizeof(char), strlen(name), f);
  
  int version = 0;
  version = htobe32(version);
  fwrite(&version, 4, 1, f);
  
  int size = 50;
  size = htobe32(size);
  fwrite(&size, 4, 1, f);
  
  int8_t x = pc.x;
  int8_t y = pc.y;
  fwrite(&x, 1, 1, f);
  fwrite(&y, 1, 1, f);
  
  int i, j;
  for(j = 0; j < WORLD_HEIGHT; j++){
    for(i = 0; i < WORLD_WIDTH; i++){
      int toWrite = d.hardness[j][i];
      fwrite(&toWrite, 1, 1, f);
    }
  }
  
  int16_t rNum = htobe16(d.roomNum);
  fwrite(&rNum, 2, 1, f);
  
  for(i = 0; i < d.roomNum; i++) {
    int8_t x = d.rooms[i].xPos;
    int8_t y = d.rooms[i].yPos;
    int8_t xs = d.rooms[i].xSize;
    int8_t ys = d.rooms[i].ySize;
    //printf("\nRoom #%d\nx: %d\ny: %d\nxSize: %d\nySize: %d\n\n", i, x, y, xs, ys);
    fwrite(&x, 1, 1, f);
    fwrite(&y, 1, 1, f);
    fwrite(&xs, 1, 1, f);
    fwrite(&ys, 1, 1, f);
  }
  
  int16_t uNum = htobe16(1);
  fwrite(&uNum, 2, 1, f);
  
  x = d.upStair.x;
  y = d.upStair.y;
  fwrite(&x, 1, 1, f);
  fwrite(&y, 1, 1, f);
  
  int16_t dNum = htobe16(1);
  fwrite(&dNum, 2, 1, f);
  
  x = d.downStair.x;
  y = d.downStair.y;
  fwrite(&x, 1, 1, f);
  fwrite(&y, 1, 1, f);
  
  
  
  fclose(f);
}

void load_file(char *filePath) {
  FILE *f;
  char marker[13];
  marker[12] = '\0';
  
  if(!(f = fopen(filePath, "rb"))) {
    fprintf(stderr, "Failed to open file for reading");
    
    return;
  }
  
  fread(marker, 12, 1, f);

  int version;
  fread(&version, 4, 1, f);
  version = be32toh(version);

  int file_size;
  fread(&file_size, 4, 1, f);
  file_size = be32toh(file_size);

  fread(&pc.x, 1, 1, f);

  fread(&pc.y, 1, 1, f);
 
  uint8_t hardness[WORLD_HEIGHT][WORLD_WIDTH];
  fread(hardness, 1, 1680, f);
  
  int i, j;
  for(j=0; j < WORLD_HEIGHT; j++){
    for(i = 0; i < WORLD_WIDTH; i++){
      d.hardness[j][i] = (int)hardness[j][i];
      if((int)hardness[j][i] == 0) {
	d.world[j][i] = CORRIDOR;
      }
    }
  }
  
  int rooms;
  fread(&rooms, 2, 1, f);
  rooms = be16toh(rooms);
  d.roomNum = rooms;
  // printf("Rooms: %d\n", rooms); 
  
  
  d.rooms = malloc(rooms * sizeof(struct Room));

  int k;
  for(i = 0; i < rooms; i++) {
    fread(&d.rooms[i].xPos, 1, 1, f);
    fread(&d.rooms[i].yPos, 1, 1, f);
    fread(&d.rooms[i].xSize, 1, 1, f);
    fread(&d.rooms[i].ySize, 1, 1, f);
    
    for(k = d.rooms[i].yPos; k < d.rooms[i].yPos + d.rooms[i].ySize; k++) {
      for(j = d.rooms[i].xPos; j < d.rooms[i].xPos + d.rooms[i].xSize; j++) {
        d.world[k][j] = ROOM;
      }
    }
  }
  
  int uCase;
  fread(&uCase, 2, 1, f);
  uCase = be16toh(uCase);
  // printf("%d\n", uCase);
  
  for(i = 0; i < uCase; i++) {
    uint8_t temp1, temp2;
    fread(&temp1, 1, 1, f);
    fread(&temp2, 1, 1, f);
    d.upStair.x = temp1;
    d.upStair.y = temp2;
    d.world[(int) temp2][(int) temp1] = UP_STAIR;
  }
 
  int dCase;
  fread(&dCase, 2, 1, f);
  dCase = be16toh(dCase);
  //printf("%d\n", dCase);
  
  for(i = 0; i < dCase; i++) {
    uint8_t temp1, temp2;
    fread(&temp1, 1, 1, f);
    fread(&temp2, 1, 1, f);
    d.downStair.x = temp1;
    d.downStair.y = temp2;
    d.world[(int) temp2][(int) temp1] = DOWN_STAIR;
  }
  
  printf("uX: %d uY: %d\ndX: %d dY:%d\n",d.upStair.x, d.upStair.y, d.downStair.x, d.downStair.y);
  printf("made it here\n");
  fclose(f);
}


void place_corridors(int roomNum){
  int i, j, k;
  //printf("mmade it to place corridors");
  struct Point closest;
  for (i = 0; i < roomNum; i++){
    closest = closest_room_to(d.rooms[i], roomNum);
    int x = d.rooms[i].xPos;
    int y = d.rooms[i].yPos;
    while(1) {
      if(x == closest.x && y == closest.y) break;
      
      if(x > closest.x) x = x - 1;
      else if (x < closest.x) x = x + 1;
      
      if(y > closest.y) y = y - 1;
      else if (y < closest.y) y = y + 1;
      
      if(d.world[y][x] == ROOM) {
        continue;
      }
      else if(d.world[y][x] == EMPTY) {
        d.world[y][x] = CORRIDOR;
        d.hardness[y][x] = 0;
      }
    }
  }
}

struct Point closest_room_to(struct Room room, int roomNum){
  struct Point temp;
  
  int x = (room.xSize % 2) + room.xPos;
  int y = (room.ySize % 2) + room.yPos;
  
  int i, j, k, minIndex = 0, minVal = INT_MAX;
  for(k = 0; k < roomNum; k++) {
    int otherX = (d.rooms[k].xSize % 2) + d.rooms[k].xPos;
    int otherY = (d.rooms[k].ySize % 2) + d.rooms[k].yPos;
    if(x == otherX && y == otherY) continue;
    int currentVal = -1 *((otherX - x) * (otherX - x) + (otherY - y) * (otherY - y));
    if (currentVal < minVal) {
       minVal = currentVal;
       minIndex = k;
       temp.x = otherX;
       temp.y = otherY;
    }
  }
  return temp;
}

int in_bounds(int xPos, int yPos, int width, int height){
  //Checks if dimensions contained within world
  return (xPos > 0) && (xPos + width < WORLD_WIDTH - 1) &&
    (yPos > 0) && (yPos + height < WORLD_HEIGHT - 1);
}


void place_rooms(int roomCount){
  //printf("made it to place_rooms");
  //Loop through the number of rooms being generated and assign dimensions to each
  int i;
  for(i = 0; i < roomCount; i++){
    int xPos, yPos, xSize, ySize;
    do{
      //Keep randomly generating dimensions until a room is found to be in the world and not overlapping
      xPos = (rand() % WORLD_WIDTH) + 1;
      yPos = (rand() % WORLD_HEIGHT) + 1;
      xSize = (rand() % (MAX_ROOM_WIDTH + 1 - MIN_ROOM_WIDTH)) + MIN_ROOM_WIDTH;
      ySize = (rand() % (MAX_ROOM_HEIGHT + 1 - MIN_ROOM_HEIGHT)) + MIN_ROOM_HEIGHT;
    }while(!in_bounds(xPos,yPos,xSize,ySize) ||
       rooms_overlap(xPos,yPos,xSize,ySize,ROOM_SPACE_BUFFER, roomCount));
       //Add dimensions to struct in roomList
       d.rooms[i].xPos = xPos;
       d.rooms[i].yPos = yPos;
       d.rooms[i].xSize = xSize;
       d.rooms[i].ySize = ySize;
       //Edit world array to show position of each room
       int j, k;
       for(j = yPos; j < yPos + ySize; j++){
          for(k = xPos; k < xPos + xSize; k++){
	     d.world[j][k] = ROOM;
	     if (i == 0) {
	       pc.x = d.rooms[i].xPos;
	       pc.y = d.rooms[i].yPos;
	     }
	     d.hardness[j][k] = 0;
          }
       }
   }
}

int rooms_overlap(int xPos, int yPos, int xSize, int ySize, int buffer, int numRooms){
  //Loops through all rooms and returns 1 if any overlap with dimensions in arguments
  int i;
  for(i = 0; i < numRooms; i++){
    struct Room *temp = &d.rooms[i];
    if(xPos < temp->xPos + temp->xSize + buffer  && xPos + xSize + buffer > temp->xPos &&
    yPos + ySize + buffer > temp->yPos && yPos < temp->yPos + temp->ySize + buffer){
      return 1;
    }
  }
  return 0;
}

void place_stairs(int numRooms){
  //printf("made it to place stairs");
  int upIndex, downIndex;
  do{
    upIndex = (rand() % numRooms);
    downIndex = (rand() % numRooms);
  }while(upIndex == downIndex);

  struct Room upRoom = d.rooms[upIndex];
  struct Room downRoom = d.rooms[downIndex];

  int upXIndex = (rand() % upRoom.xSize);
  int upYIndex = (rand() % upRoom.ySize);
  int downXIndex = (rand() % downRoom.xSize);
  int downYIndex = (rand() % downRoom.ySize);
  
  d.upStair.x = upXIndex + upRoom.xPos;
  d.upStair.y = upYIndex + upRoom.yPos;
  d.world[upYIndex + upRoom.yPos][upXIndex + upRoom.xPos] = UP_STAIR;
  
  d.downStair.x = downXIndex + downRoom.xPos;
  d.downStair.y = downYIndex + downRoom.yPos;
  d.world[downYIndex + downRoom.yPos][downXIndex + downRoom.xPos] = DOWN_STAIR;
}

void generate_random(){
  d.roomNum = (rand() % (MAX_ROOMS + 1 - MIN_ROOMS)) + MIN_ROOMS;
  d.rooms = malloc(d.roomNum * sizeof(struct Room));
  
  for (int i = 0; i < WORLD_HEIGHT; i++) {
    for (int j = 0; j < WORLD_WIDTH; j++) {
      d.hardness[i][j] = 255;
    }
  }
 
  place_rooms(d.roomNum);
  place_corridors(d.roomNum);
  place_stairs(d.roomNum);
}



void print_world(int world[WORLD_HEIGHT][WORLD_WIDTH]){
  int i, j;
  printf(" -------------------------------------------------------------------------------- \n");
  for(j = 0; j < WORLD_HEIGHT; j++){
    printf("|");
    for(i = 0; i < WORLD_WIDTH; i++){

       if(pc.x == i && pc.y == j) {
	 printf("@");
          continue;
       }
       if(world[j][i] == EMPTY){
          printf(" ");
       }
       if(world[j][i] == ROOM) {
          printf(".");
       }
       if(world[j][i] == CORRIDOR){
          printf("#");
       }
       if(world[j][i]==DOWN_STAIR){
	 printf(">");
       }
       if(world[j][i]==UP_STAIR){
	 printf("<");
       }
       
    }
    printf("|\n");
  }
  printf(" -------------------------------------------------------------------------------- \n");
}

void clean_up(){
  free(d.rooms);
}

Node* newNode(int x , int y , int p)
{
    Node* temp = (Node*)malloc(sizeof(Node));
    temp->data[0] = x;
    temp->data[1] = y;
    temp->prio = p;
    temp->next = NULL;

    return temp;
}

int * peek(Node** head)
{
    return (*head)->data;
}

void pop(Node** head)
{
    Node* temp = *head;
    (*head) = (*head)->next;
    free(temp);
}


void push(Node** head, int d[2], int p)
{

    Node* temp = newNode( d[0] , d[1] , p );
    Node* start = (*head);
    if ((*head)->prio > p)
    {
        temp->next = *head;
        (*head) = temp;
    }
    else
    {

       while (start->next != NULL && start->next->prio < p)
       {
           start = start->next;
       }

        temp->next = start->next;
        start->next = temp;
    }
}

int isEmpty(Node** head)
{
    return (*head) == NULL;
}

void boringDist()
{
	Node* q = newNode( pc.x , pc.y , 0 );

	int i;
	int j;

	for( i = 0 ; i < 21 ; i += 1 )
	{
		for( j = 0 ; j < 80 ; j += 1 )
		{
			d.boreDist[i][j] = 10000;
		}
	}

	d.boreDist[pc.x][pc.y] = 0;

	while(!isEmpty(&q))
	{
		int cur[2];

		cur[0] = peek(&q)[0];
		cur[1] = peek(&q)[1];

		pop(&q);

		int y;
		int x;

		for( y = -1 ; y <= 1 ; y += 1 )
		{
			for( x = -1 ; x <= 1 ; x += 1 )
			{
				if( (cur[0] + y) < 21 && (cur[0] + y) >= 0 && (cur[1] + x) >= 0 && (cur[1] + x) < 80 )
				{
					if(d.boreDist[cur[0] + y][cur[1] + x] > d.boreDist[cur[0]][cur[1]] + 1 + (hardness[cur[0] + y][cur[1] + x] / 85 ) && hardness[cur[0] + y][cur[1] + x] < 255)
					{
						d.boreDist[cur[0] + y][cur[1] + x] = d.boreDist[cur[0]][cur[1]] + 1 + (hardness[cur[0] + y][cur[1] + x] / 85 );
						int po[] = { cur[0] + y , cur[1] + x };
						push( &q , po , boreDist[cur[0] + y][cur[1] + x] );
					}
				}
			}
		}
	}

       
}


void walkingDist()
{
	Node* q = newNode( pc.x , pc.y , 0 );

	int i;
	int j;

	for( i = 0 ; i < 21 ; i += 1 )
	{
		for( j = 0 ; j < 80 ; j += 1 )
		{
				d.walkDist[i][j] = 10000;
		}
	}

	d.walkDist[pc.x][pc.y] = 0;

	while(!isEmpty(&q))
	{
		int cur[2];

		cur[0] = peek(&q)[0];
		cur[1] = peek(&q)[1];

		pop(&q);

		int y;
		int x;

		for( y = -1 ; y <= 1 ; y += 1 )
		{
			for( x = -1 ; x <= 1 ; x += 1 )
			{
				if( (cur[0] + y) < 21 && (cur[0] + y) >= 0 && (cur[1] + x) >= 0 && (cur[1] + x) < 80 )
				{
					if(d.walkDist[cur[0] + y][cur[1] + x] > d.walkDist[cur[0]][cur[1]] + 1 && world[cur[0] + y][cur[1] + x] >= 1)
					{
						d.walkDist[cur[0] + y][cur[1] + x] = d.walkDist[cur[0]][cur[1]] + 1;
						int po[] = { cur[0] + y , cur[1] + x };
						push( &q , po , d.walkDist[cur[0] + y][cur[1] + x] );
					}
				}
			}
		}
	}

}

