#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include "project1-01.h"

struct Room{
   int xPos;
   int yPos;
   int xSize;
   int ySize;
};

struct Dungeon{
   int world[WORLD_HEIGHT][WORLD_WIDTH];
   int hardness[WORLD_HEIGHT][WORLD_WIDTH];
   struct Room rooms[MAX_ROOMS];
};

struct Dungeon d;

int main(int argc, char *argv[]){

  srand(time(0));
  int rooms = (rand() % (MAX_ROOMS + 1 - MIN_ROOMS)) + MIN_ROOMS;
  printf("%d\n", rooms);
  

  place_rooms(rooms);
  //generate_world(world);
  print_world(d.world);
  
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




void print_world(int world[WORLD_HEIGHT][WORLD_WIDTH]){
  int i, j;
  printf(" -------------------------------------------------------------------------------- \n");
  for(j = 0; j < WORLD_HEIGHT; j++){
    printf("|");
    for(i = 0; i < WORLD_WIDTH; i++){
       if(world[j][i] == EMPTY){
          printf(" ");
       }
       if(world[j][i] == ROOM) {
          printf(".");
       }
       if(world[j][i] == CORRIDOR){
          printf("#");
       }
    }
    printf("|\n");
  }
  printf(" -------------------------------------------------------------------------------- \n");
}
