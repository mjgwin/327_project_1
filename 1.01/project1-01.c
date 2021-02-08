#include <stdio.h>
#include <stdlib.h>
#include "project1-01.h"

int main(int argc, char *argv[]){

  char **world;
  //Allocate space for each row
  world = (char **)malloc(WORLD_HEIGHT * sizeof(char *));
  int i;
  //Allocate space based on size of row for all rows
  for(i = 0; i < WORLD_HEIGHT; i++){
    world[i] = (char *)malloc(WORLD_WIDTH * sizeof(char));
  }
  fill_edges(world);
  generate_world(world);
  print_world(world);
  clean_up(world);
}

void fill_edges(char **world){
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

void generate_world(char **world){
  //Placeholder for now but world generation algorithm will go here
  int i, j;
  for(j = 1; j < WORLD_HEIGHT - 1; j++){
    for(i = 1; i < WORLD_WIDTH - 1; i++){
      world[j][i] = ' ';
    }
  }
}

void print_world(char **world){
  //Display world in console (only displays correctly when terminal is 80x24)
  int i, j;
  for(j = 0; j < WORLD_HEIGHT; j++){
    for(i = 0; i < WORLD_WIDTH; i++){
      printf("%c", world[j][i]);
    }
  }
}

void clean_up(char **world){
  //Memory management to avoid any leaks
  int i;
  for(i = 0; i < WORLD_HEIGHT; i++){
    free(world[i]);
  }

  free(world);
}
