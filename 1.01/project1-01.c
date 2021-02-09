#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "project1-01.h"

//Room struct that contains data with dimensions
typedef struct Room{
  int xPos;
  int yPos;
  int width;
  int height;
} Room;
//Global world 2d array
char **world;
//Global list of rooms
Room *roomList;

int main(int argc, char *argv[]){
  //Seed random number generator
  srand(time(0));
  
  //Allocate space for each row
  world = (char **)malloc(WORLD_HEIGHT * sizeof(char *));
  //Allocate space based on size of row for all rows
  int i;
  for(i = 0; i < WORLD_HEIGHT; i++){
    world[i] = (char *)malloc(WORLD_WIDTH * sizeof(char));
  }
  fill_edges();
  generate_world();
  
  //Get number of rooms to generate and allocate space in room list
  int roomsToPlace = (rand() % (MAX_ROOMS + 1 - MIN_ROOMS)) + MIN_ROOMS;
  roomList = (Room *)malloc(roomsToPlace * sizeof(Room));
  
  
  place_rooms(roomsToPlace);
  print_world();
  clean_up();
}

void fill_edges(){
 //Add a border around edges
 int i ,j;
 for(j = 0; j < WORLD_HEIGHT; j++){
    for(i = 0; i < WORLD_WIDTH; i++){
      if(j == 0 || j == WORLD_HEIGHT - 1){
	world[j][i] = '-';
      }
      if(i == 0 || i == WORLD_WIDTH - 1){
	world[j][i] = '|';
      }
    }
  }
}

void generate_world(){
  //Placeholder for now but world generation algorithm will go here
  int i, j;
  for(j = 1; j < WORLD_HEIGHT - 1; j++){
    for(i = 1; i < WORLD_WIDTH - 1; i++){
      world[j][i] = ' ';
    }
  }
}

void place_rooms(int roomCount){
  //Loop through the number of rooms being generated and assign dimensions to each
  int i;
  for(i = 0; i < roomCount; i++){
    int x, y, width, height;
    do{
      //Keep randomly generating dimensions until a room is found to be in the world and not overlapping
      x = (rand() % WORLD_WIDTH) + 1;
      y = (rand() % WORLD_HEIGHT) + 1;
      width = (rand() % (MAX_ROOM_WIDTH + 1 - MIN_ROOM_WIDTH)) + MIN_ROOM_WIDTH;
      height = (rand() % (MAX_ROOM_HEIGHT + 1 - MIN_ROOM_HEIGHT)) + MIN_ROOM_HEIGHT;
    }while(!in_bounds(x,y,width,height) ||
     rooms_overlap(x,y,width,height,ROOM_SPACE_BUFFER, roomCount));
    //Add dimensions to struct in roomList
    roomList[i].xPos = x;
    roomList[i].yPos = y;
    roomList[i].width = width;
    roomList[i].height = height;
    //Edit world array to show position of each room
    int j, k;
    for(j = y; j <= y + height; j++){
      for(k = x; k <= x + width; k++){
	world[j][k] = '#';
      }
    }
  }
}

int rooms_overlap(int xPos, int yPos, int width, int height, int buffer, int numRooms){
  //Loops through all rooms and returns 1 if any overlap with dimensions in arguments
  int i;
  for(i = 0; i < numRooms; i++){
    Room *temp = &roomList[i];
    if(xPos < temp->xPos + temp->width + buffer  && xPos + width + buffer > temp->xPos &&
    yPos + height + buffer > temp->yPos && yPos < temp->yPos + temp->height + buffer){
      return 1;
    }
  }
  return 0;
}

int in_bounds(int xPos, int yPos, int width, int height){
  //Checks if dimensions contained within world
  return (xPos > 0) && (xPos + width < WORLD_WIDTH - 1) &&
    (yPos > 0) && (yPos + height < WORLD_HEIGHT - 1);
}

void print_world(){
  //Display world in console (only displays correctly when terminal is 80x24)
  int i, j;
  for(j = 0; j < WORLD_HEIGHT; j++){
    for(i = 0; i < WORLD_WIDTH; i++){
      printf("%c", world[j][i]);
    }
  }
}

void clean_up(){
  //Memory management to avoid any leaks
  int i;
  for(i = 0; i < WORLD_HEIGHT; i++){
    free(world[i]);
  }

  free(world);
  free(roomList);
}
