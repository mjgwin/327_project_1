#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include "project1-01.h"


int main(int argc, char *argv[]){

  srand(time(0));
  int rooms = (rand() % (MAX_ROOMS + 1 - MIN_ROOMS)) + MIN_ROOMS;
  printf("%d\n", rooms);
  

  place_rooms(rooms);
  place_corridors(rooms);
  place_stairs(rooms);
  print_world(d.world);
  
  
}

void place_corridors(int roomNum){
  int i, j, k;
  struct Point closest;
  for (i = 1; i < roomNum; i++){
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
  printf("Picked rooms with dimensions %d, %d and %d,%d", upRoom.xSize, upRoom.ySize, downRoom.xSize, downRoom.ySize);
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
