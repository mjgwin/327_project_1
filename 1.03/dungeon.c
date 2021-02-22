#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "project1-01.h"


int main(int argc, char *argv[]){

  srand(time(0));
  int rooms = (rand() % (MAX_ROOMS + 1 - MIN_ROOMS)) + MIN_ROOMS;
  
  char *filePath = strcat(getenv("HOME"), "/cs327/327_project_1/1.02/testfiles/adventure.rlg327");
  printf("%s\n", filePath);
  //mkdir(filePath, 0777);
  
  //filePath = strcat(filePath, "/dungeon");
  
  load_file(filePath);
  /*
  if (argc == 1) {
    place_rooms(rooms);
    place_corridors(rooms);
    place_stairs(rooms);
    print_world(d.world);
    return 0;
  }
  
  if (argc == 2) {
    if(!strcmp(argv[1], "--save")) {
      place_rooms(rooms);
      place_corridors(rooms);
      place_stairs(rooms);
      print_world(d.world);
      printf("--save flag is working\n");
    }
    else if(!strcmp(argv[1], "--load")) {
      printf("--load flag is working\n");
    }
    else {
      printf("error: flag not recognized
    }
  }*/
  return 0;
  
}


void save_file(char *filePath) {
  FILE *f;
  if(!(f = fopen(filePath, "w"))) {
    fprintf(stderr, "Failed to open file for writing");
    
    return;
  }
  
  char *name = "RLG327-S2021";
  fwrite(name, sizeof(char), strlen(name), f);
  
  
  
  
  fclose(f);
}

void load_file(char *filePath) {
  FILE *f;
  char buffer[100] = {'\0'};
  
  //opens file in reading binary mode
  if(!(f = fopen(filePath, "rb"))) {
    fprintf(stderr, "Failed to open file for reading");
    
    return;
  }
  
  //reads and prints the file type marker
  fread(buffer, 12, 1, f);
  //printf("%s\n", buffer);
  
  //reads file version marker
  uint32_t a;
  fread(&a, sizeof(uint32_t), 1, f);
  //printf("%d\n", a);
  
  //reads size of the file
  uint32_t b;
  fread(&b, sizeof(uint32_t), 1, f);
  //printf("%d\n", b);
  
  //reads the x and y position of the PC
  uint8_t x;
  fread(&x, sizeof(uint8_t), 1, f);
  pc.x = (int) x;
  //printf("pc.x = %d\n", pc.x);
  
  uint8_t y;
  fread(&y, sizeof(uint8_t), 1, f);
  pc.y = (int) y;
  //printf("pc.y = %d\n", pc.y);
  
  //reads given hardness values and assigns them to global variable
  int i, count = 0;
  for (i = 0; i < 1680; i++) {
    uint8_t val;
    fread(&val, sizeof(uint8_t), 1, f);
    
    //in this condition whenever the hardness is zero it assigns the same space on the world array with CORRIDOR
    //this works because we overwrite to ROOM later!
    if((int) val == 0) {
      d.world[i / WORLD_WIDTH][i % WORLD_WIDTH] = CORRIDOR;
    }
    
    
    d.hardness[i / WORLD_WIDTH][i % WORLD_WIDTH] = (int) val;
    
    //for debugging
    //printf("%d: %d\n", count++, d.hardness[i / WORLD_WIDTH][i % WORLD_WIDTH]);
  }
  
  //reads the number of rooms
  uint16_t room;
  fread(&room, sizeof(uint16_t), 1, f);
  int rooms;
  rooms = (int) room;
  //for some reason the rooms were all multiplied ny a factor of 256 not sure why yet
  rooms = rooms / 256;
  //for debigging
  printf("%d\n", rooms); 
  
  
  //TODO: make rooms dynamically allocate memory
  //d.rooms = malloc(rooms * sizeof(struct Room)); note: I do not think that this is correct
  
  
  
  //assigns proper values for each room in the global variable
  uint8_t xPos, yPos, xSize, ySize;
  int j, k;
  for(i = 0; i < rooms; i++) {
    fread(&xPos, sizeof(uint8_t), 1, f);
    d.rooms[i].xPos = (int) xPos;
    fread(&yPos, sizeof(uint8_t), 1, f);
    d.rooms[i].yPos = (int) yPos;
    fread(&xSize, sizeof(uint8_t), 1, f);
    d.rooms[i].xSize = (int) xSize;
    fread(&ySize, sizeof(uint8_t), 1, f);
    d.rooms[i].ySize = (int) ySize;
    for(k = d.rooms[i].yPos; k < d.rooms[i].yPos + d.rooms[i].ySize; k++) {
      for(j = d.rooms[i].xPos; j < d.rooms[i].xPos + d.rooms[i].xSize; j++) {
        d.world[k][j] = ROOM;
      }
    }
    
    //very useful for debugging
    //printf("Room #%d\nxPos: %d\nyPos: %d\nxSize: %d\nySize: %d\n\n", i + 1, d.rooms[i].xPos, d.rooms[i].yPos, d.rooms[i].xSize, d.rooms[i].ySize);
  }
  
  //reads the number of upward stair cases
  uint16_t uCase;
  fread(&uCase, 2, 1, f);
  int upCase = (int) uCase / 256;
  
  //assigns stair cases to world map
  for(i = 0; i < upCase; i++) {
    uint8_t temp1, temp2;
    fread(&temp1, 1, 1, f);
    fread(&temp2, 1, 1, f);
    d.world[(int) temp2][(int) temp1] = UP_STAIR;
  }
  
  //reads the number of downward stair cases
  uint16_t dCase;
  fread(&uCase, 2, 1, f);
  int downCase = (int) uCase / 256;
  
  //assigns downward staircases to world map
  for(i = 0; i < downCase; i++) {
    uint8_t temp1, temp2;
    fread(&temp1, 1, 1, f);
    fread(&temp2, 1, 1, f);
    d.world[(int) temp2][(int) temp1] = DOWN_STAIR;
  }
  
  fclose(f);
  
  
  
  print_world(d.world);

  
  
}


void place_corridors(int roomNum){
  int i, j, k;
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
       for(j = xPos; j <= xPos + xSize; j++){
          for(k = yPos; k <= yPos + ySize; k++){
	     d.world[k][j] = ROOM;
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
  
  d.world[upYIndex + upRoom.yPos][upXIndex + upRoom.xPos] = UP_STAIR;
  d.world[downYIndex + downRoom.yPos][downXIndex + downRoom.xPos] = DOWN_STAIR;
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
