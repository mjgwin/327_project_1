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

int main(int argc, char *argv[]){

  srand(time(0));
  int rooms = (rand() % 6) + 3;
  printf("%d\n", rooms);
  struct Dungeon d;

  
  int i;
  
  for(i = 0; i < rooms; i++) {
     printf("Iteration\n");
     d.rooms[i].xPos = rand() % WORLD_WIDTH;
     printf("%d\n", d.rooms[i].xPos);
     d.rooms[i].yPos = rand() % WORLD_HEIGHT;
     printf("%d\n", d.rooms[i].yPos);
     d.rooms[i].xSize = (rand() % 5) + 3;
     printf("%d\n", d.rooms[i].xSize);
     d.rooms[i].ySize = (rand() % 5) + 3;
     printf("%d\n", d.rooms[i].ySize);
  }
  
  for(i = 0; i < rooms; i++){
     int j, k;
     for(j = d.rooms[i].xPos; j < d.rooms[i].xPos + d.rooms[i].xSize; j++) {
        for(k = d.rooms[i].yPos; k < d.rooms[i].yPos + d.rooms[i].ySize; k++) {
           d.world[k][j] = ROOM;
        }
     }
  }
  
  
  
  

  //generate_world(world);
  print_world(d.world);
  
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
