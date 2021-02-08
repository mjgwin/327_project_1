#include <stdio.h>
#include <stdlib.h>
#include "project1-01.h"

int main(int argc, char *argv[]){

  char **world;

  world = (char **)malloc(WORLD_HEIGHT * sizeof(char *));
  int i;
  for(i = 0; i < WORLD_HEIGHT; i++){
    world[i] = (char *)malloc(WORLD_WIDTH * sizeof(char));
  }
  fill_edges(world);
  generate_world(world);
  print_world(world);
  
}

void fill_edges(char **world){
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
  int i, j;
  for(j = 1; j < WORLD_HEIGHT - 1; j++){
    for(i = 1; i < WORLD_WIDTH - 1; i++){
      world[j][i] = ' ';
    }
  }
}

void print_world(char **world){
  int i, j;
  for(j = 0; j < WORLD_HEIGHT; j++){
    for(i = 0; i < WORLD_WIDTH; i++){
      printf("%c", world[j][i]);
    }
  }
}
