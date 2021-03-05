#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>

#include "dungeon.h"
#include "path.h"



static int32_t mon_cmp(const void *key, const void *with) {
  int32_t cmp = ((mon_t *) key)->nextTurn - ((mon_t *) with)->nextTurn;
  if (cmp == 0) {
    cmp = ((mon_t *) key)->prio - ((mon_t *) with)->prio;
  }
  return cmp;
}

int in_room(dungeon_t *d, mon_t *mon)
{
  int i;
  int inRoom;
  for (i = 0; i < d->num_rooms; i++) {
    if ((d->pc.position[dim_x] >= d->rooms[i].position[dim_x]) && (d->pc.position[dim_x] < (d->rooms[i].position[dim_x] + d->rooms[i].size[dim_x])) && (d->pc.position[dim_y] >= d->rooms[i].position[dim_y]) && (d->pc.position[dim_y] < (d->rooms[i].position[dim_y] + d->rooms[i].size[dim_y]))) {
      inRoom = i;
    }
  }
  
  if ((mon->position[dim_x] >= d->rooms[inRoom].position[dim_x]) && (mon->position[dim_x] < (d->rooms[inRoom].position[dim_x] + d->rooms[inRoom].size[dim_x])) && (mon->position[dim_y] >= d->rooms[inRoom].position[dim_y]) && (mon->position[dim_y] < (d->rooms[inRoom].position[dim_y] + d->rooms[inRoom].size[dim_y]))) {
      return 1;
    }
  return 0;
}

void usage(char *name)
{
  fprintf(stderr,
          "Usage: %s [-r|--rand <seed>] [-l|--load [<file>]]\n"
          "          [-s|--save [<file>]] [-i|--image <pgm file>]\n",
          name);

  exit(-1);
}

terrain_type_t set_mon_id(mon_t *mon){
  switch(mon->c){
  case '0':
     return ter_mon_0;
     break;
  case '1':
     return ter_mon_1;
     break;
  case '2':
     return ter_mon_2;
     break;
  case '3':
     return ter_mon_3;
     break;
  case '4':
     return ter_mon_4;
     break;
  case '5':
     return ter_mon_5;
     break;
  case '6':
     return ter_mon_6;
     break;
  case '7':
     return ter_mon_7;
     break;
  case '8':
     return ter_mon_8;
     break;
  case '9':
     return ter_mon_9;
     break;
  case 'a':
     return ter_mon_a;
     break;
  case 'b':
     return ter_mon_b;
     break;
  case 'c':
     return ter_mon_c;
     break;
  case 'd':
     return ter_mon_d;
     break;
  case 'e':
     return ter_mon_e;
     break;
  case 'f':
     return ter_mon_f;
     break;
  }
  return ter_mon_0;
}

int mons_overlap(mon_t *first, mon_t *second){
  return first->position[dim_y] ==  second->position[dim_y] &&
    first->position[dim_x] == second->position[dim_x]; 
}

int overlaps_with_any(dungeon_t *d, mon_t *mon){
  int i;
  for(i = 0; i < d->num_mons; i++){
    mon_t *temp = &d->mons[i];
    if(mons_overlap(mon, temp)){
      return 1;
    }
  }

  return 0;
}


void generate_pc(dungeon_t *d){
  mon_t *player = &d->mons[0];
  player->position[dim_y] = d->pc.position[dim_y];
  player->position[dim_x] = d->pc.position[dim_x];
  player->nextTurn = 0;
  player->speed = 10;
  player->prio = 0;
  player->c = '@';
}

//dungeon, mon to generate, intelligence flag, telepathy flag, tunneling flag, erratic flag, priority
void generate_mon(dungeon_t *d, mon_t *mon, uint32_t intel, uint32_t tele, uint32_t tunn, uint32_t err, int prio){
  int speed = (rand() % 15) + 5;
  mon->nextTurn = 0;
  mon->speed = speed;
  mon->prio = prio;

  if(intel && tele && tunn && err){
    mon->c = 'f';
  }else if(intel && tele && tunn && !err){
    mon->c = '7';
  }else if(intel && tele && !tunn && err){
    mon->c = 'b';
  }else if(intel && tele && !tunn && !err){
    mon->c = '3';
  }else if(intel && !tele && tunn && err){
    mon->c = 'd';
  }else if(intel && !tele && tunn && !err){
    mon->c = '5';
  }else if(intel && !tele && !tunn && err){
    mon->c = '9';
  }else if(intel && !tele && !tunn && !err){
    mon->c = '1';
  }else if(!intel && tele && tunn && err){
    mon->c = 'e';
  }else if(!intel && tele && tunn && !err){
    mon->c = '6';
  }else if(!intel && tele && !tunn && err){
    mon->c = 'a';
  }else if(!intel && tele && !tunn && !err){
    mon->c = '2';
  }else if(!intel && !tele && tunn && err){
    mon->c = 'c';
  }else if(!intel && !tele && tunn && !err){
    mon->c = '4';
  }else if(!intel && !tele && !tunn && err){
    mon->c = '8';
  }else if(!intel && !tele && !tunn && !err){
    mon->c = '0';
  }
  
  if(tele && tunn){
    do{
       int spawnY = rand() % (DUNGEON_Y - 1);
       int spawnX = rand() % (DUNGEON_X - 1);
       mon->position[dim_y] = spawnY;
       mon->position[dim_x] = spawnX;
    }while(overlaps_with_any(d, mon));
   
  }else{
    do{
       int spawnRoom = rand() % d->num_rooms;
       room_t temp = d->rooms[spawnRoom];
       int roomY = temp.position[dim_y] + rand() % temp.size[dim_y];
       int roomX = temp.position[dim_x] + rand() % temp.size[dim_x];
       mon->position[dim_y] = roomY;
       mon->position[dim_x] = roomX;
    }while(in_room(d, mon) || overlaps_with_any(d, mon));
  }
  
  mon->memory[dim_y] = mon->position[dim_y];
  mon->memory[dim_x] = mon->position[dim_x];

  mon->id = set_mon_id(mon);
}

void generate_mons(dungeon_t *d, int numMons){
  //Plus 1 for player mon
  d->num_mons = numMons + 1;
  d->mons = malloc(sizeof(mon_t) * (numMons + 1));
  generate_pc(d);
  uint32_t intel, tele, tunn, err;
  int i;
  for(i = 1; i < numMons; i++){
    intel = rand() % 2;
    tele = rand() % 2;
    tunn = rand() % 2;
    err = rand() % 2;
    generate_mon(d, &d->mons[i], intel, tele, tunn, err, i);
  }
}



void monster_loop(dungeon_t *d){
  heap_t h;
  heap_init(&h, mon_cmp, NULL);

  int currMon;
  for(currMon = 0; currMon < d->num_mons; currMon++){
    mon_t *temp = &d->mons[currMon];
    temp->prev = mapxy(temp->position[dim_x], temp->position[dim_y]);
    mapxy(temp->position[dim_x], temp->position[dim_y]) = temp->id;
    temp->hn = heap_insert(&h, temp);
  }
   
  dijkstra(d);
  dijkstra_tunnel(d);
  render_dungeon(d);

  srand(time(0));
  
  mon_t *removed = malloc(sizeof(mon_t));
  while(1) {
    removed = (mon_t *) heap_remove_min(&h);
    char is = removed->c;
    int x, y;
    if (is == '@') {
      removed->nextTurn = removed->nextTurn + (1000 / removed->speed);
      removed->hn = heap_insert(&h, removed);
      render_dungeon(d);
      usleep(250000);
    }
    else if (is == '0') {
    }
    else if (is == '1') {
    }
    else if (is == '2') {
      pair_t pMin;
      pMin[dim_x] = removed->position[dim_x];
      pMin[dim_y] = removed->position[dim_y];
      if(removed->position[dim_x] < d->pc.position[dim_x]) {
        ++pMin[dim_x];
      }
      else if (removed->position[dim_x] > d->pc.position[dim_x]) {
        --pMin[dim_x];
      }
      
      if(removed->position[dim_y] < d->pc.position[dim_y]) {
        ++pMin[dim_y];
      }
      else if (removed->position[dim_y] > d->pc.position[dim_y]) {
        --pMin[dim_y];
      }
      
      if(d->hardness[pMin[dim_y]][pMin[dim_x]] == 0) {
        mapxy(removed->position[dim_x], removed->position[dim_y]) = removed->prev;
        removed->prev = mapxy(pMin[dim_x], pMin[dim_y]);
        mapxy(pMin[dim_x], pMin[dim_y]) = ter_mon_2;
        removed->position[dim_x] = pMin[dim_x];
        removed->position[dim_y] = pMin[dim_y];
      }
      removed->nextTurn = removed->nextTurn + (1000 / removed->speed);
      removed->hn = heap_insert(&h, removed);
      if(pMin[dim_x] == d->pc.position[dim_x] && pMin[dim_y] == d->pc.position[dim_y]) {
        d->pc.position[dim_x] = 0;
        d->pc.position[dim_y] = 0;
        render_dungeon(d);
        break;
      } 
    }
    else if (is == '3') {
      pair_t pMin;
      int iMin = INT_MAX;
      for(y = removed->position[dim_y] - 1; y < removed->position[dim_y] + 2; y++) {
        for(x = removed->position[dim_x] - 1; x < removed->position[dim_x] + 2; x++) {
          if(d->pc_distance[y][x] < iMin) {
            iMin = d->pc_distance[y][x];
            pMin[dim_x] = x;
            pMin[dim_y] = y;
          }
        }
      }
      mapxy(removed->position[dim_x], removed->position[dim_y]) = removed->prev;
      removed->prev = mapxy(pMin[dim_x], pMin[dim_y]);
      mapxy(pMin[dim_x], pMin[dim_y]) = ter_mon_3;
      removed->position[dim_x] = pMin[dim_x];
      removed->position[dim_y] = pMin[dim_y];
      removed->nextTurn = removed->nextTurn + (1000 / removed->speed);
      removed->hn = heap_insert(&h, removed);
      if(pMin[dim_x] == d->pc.position[dim_x] && pMin[dim_y] == d->pc.position[dim_y]) {
        d->pc.position[dim_x] = 0;
        d->pc.position[dim_y] = 0;
        render_dungeon(d);
        break;
      }
    }
    else if (is == '4') {
    }
    else if (is == '5') {
    }
    else if (is == '6') {
      pair_t pMin;
      pMin[dim_x] = removed->position[dim_x];
      pMin[dim_y] = removed->position[dim_y];
      if(removed->position[dim_x] < d->pc.position[dim_x]) {
        ++pMin[dim_x];
      }
      else if (removed->position[dim_x] > d->pc.position[dim_x]) {
        --pMin[dim_x];
      }
      
      if(removed->position[dim_y] < d->pc.position[dim_y]) {
        ++pMin[dim_y];
      }
      else if (removed->position[dim_y] > d->pc.position[dim_y]) {
        --pMin[dim_y];
      }
      
      if(d->hardness[pMin[dim_y]][pMin[dim_x]] > 85) {
        d->hardness[pMin[dim_y]][pMin[dim_x]] = d->hardness[pMin[dim_y]][pMin[dim_x]] - 85;
        dijkstra_tunnel(d);
      }
      else {
        d->hardness[pMin[dim_y]][pMin[dim_x]] = 0;
        if (removed->prev == ter_wall) {
          mapxy(removed->position[dim_x], removed->position[dim_y]) = ter_floor_hall;
          removed->position[dim_x] = pMin[dim_x];
          removed->position[dim_y] = pMin[dim_y];
          removed->prev = mapxy(pMin[dim_x], pMin[dim_y]);
          mapxy(pMin[dim_x], pMin[dim_y]) = ter_mon_6;
          dijkstra(d);
          dijkstra_tunnel(d);
        }
        else {
          mapxy(removed->position[dim_x], removed->position[dim_y]) = removed->prev;
          removed->prev = mapxy(pMin[dim_x], pMin[dim_y]);
          mapxy(pMin[dim_x], pMin[dim_y]) = ter_mon_6;
          removed->position[dim_x] = pMin[dim_x];
          removed->position[dim_y] = pMin[dim_y];
        }
      }
      removed->nextTurn = removed->nextTurn + (1000 / removed->speed);
      removed->hn = heap_insert(&h, removed);
      if(pMin[dim_x] == d->pc.position[dim_x] && pMin[dim_y] == d->pc.position[dim_y]) {
        d->pc.position[dim_x] = 0;
        d->pc.position[dim_y] = 0;
        render_dungeon(d);
        break;
      }
    }
    else if (is == '7') {
      pair_t pMin;
      int iMin = INT_MAX;
      for(y = removed->position[dim_y] - 1; y < removed->position[dim_y] + 2; y++) {
        for(x = removed->position[dim_x] - 1; x < removed->position[dim_x] + 2; x++) {
          if(d->pc_tunnel[y][x] < iMin) {
            iMin = d->pc_tunnel[y][x];
            pMin[dim_x] = x;
            pMin[dim_y] = y;
          }
        }
      }
      
      if(d->hardness[pMin[dim_y]][pMin[dim_x]] > 85) {
        d->hardness[pMin[dim_y]][pMin[dim_x]] = d->hardness[pMin[dim_y]][pMin[dim_x]] - 85;
        dijkstra_tunnel(d);
      }
      else {
        d->hardness[pMin[dim_y]][pMin[dim_x]] = 0;
        if (removed->prev == ter_wall) {
          mapxy(removed->position[dim_x], removed->position[dim_y]) = ter_floor_hall;
          removed->position[dim_x] = pMin[dim_x];
          removed->position[dim_y] = pMin[dim_y];
          removed->prev = mapxy(pMin[dim_x], pMin[dim_y]);
          mapxy(pMin[dim_x], pMin[dim_y]) = ter_mon_7;
          dijkstra(d);
          dijkstra_tunnel(d);
        }
        else {
          mapxy(removed->position[dim_x], removed->position[dim_y]) = removed->prev;
          removed->prev = mapxy(pMin[dim_x], pMin[dim_y]);
          mapxy(pMin[dim_x], pMin[dim_y]) = ter_mon_7;
          removed->position[dim_x] = pMin[dim_x];
          removed->position[dim_y] = pMin[dim_y];
        }
      }
      removed->nextTurn = removed->nextTurn + (1000 / removed->speed);
      removed->hn = heap_insert(&h, removed);
      if(pMin[dim_x] == d->pc.position[dim_x] && pMin[dim_y] == d->pc.position[dim_y]) {
        d->pc.position[dim_x] = 0;
        d->pc.position[dim_y] = 0;
        render_dungeon(d);
        break;
      }
    }
    else if (is == '8') {
    }
    else if (is == '9') {
    }
    else if (is == 'a') {
      int erratic = rand() % 2;
      pair_t pMin;
      if (erratic) {
        printf("erratic behavior selected\n");
        while(1) {
          pMin[dim_x] = (rand() % 3) + (removed->position[dim_x] - 1);
          pMin[dim_y] = (rand() % 3) + (removed->position[dim_y] - 1);
          if(pMin[dim_x] == removed->position[dim_x] && pMin[dim_y] == removed->position[dim_y]) {
            continue;
          }
          if(d->hardness[pMin[dim_y]][pMin[dim_x]] == 0) {
            break;
          }
        }
        printf("x: %d, y: %d\n", removed->position[dim_x], removed->position[dim_y]);
        printf("x: %d, y: %d\n", pMin[dim_x], pMin[dim_y]); 
      } else {
        pMin[dim_x] = removed->position[dim_x];
        pMin[dim_y] = removed->position[dim_y];
        if(removed->position[dim_x] < d->pc.position[dim_x]) {
          ++pMin[dim_x];
        }
        else if (removed->position[dim_x] > d->pc.position[dim_x]) {
          --pMin[dim_x];
        }
      
        if(removed->position[dim_y] < d->pc.position[dim_y]) {
          ++pMin[dim_y];
        }
        else if (removed->position[dim_y] > d->pc.position[dim_y]) {
          --pMin[dim_y];
        }
      }
      if(d->hardness[pMin[dim_y]][pMin[dim_x]] == 0) {
        mapxy(removed->position[dim_x], removed->position[dim_y]) = removed->prev;
        removed->prev = mapxy(pMin[dim_x], pMin[dim_y]);
        mapxy(pMin[dim_x], pMin[dim_y]) = ter_mon_a;
        removed->position[dim_x] = pMin[dim_x];
        removed->position[dim_y] = pMin[dim_y];
      }
      removed->nextTurn = removed->nextTurn + (1000 / removed->speed);
      removed->hn = heap_insert(&h, removed);
      if(pMin[dim_x] == d->pc.position[dim_x] && pMin[dim_y] == d->pc.position[dim_y]) {
        d->pc.position[dim_x] = 0;
        d->pc.position[dim_y] = 0;
        render_dungeon(d);
        break;
      }
    }
    else if (is == 'b') {
      int erratic = rand() % 2;
      pair_t pMin;
      int iMin = INT_MAX;
      if (erratic) {
        while(1) {
          pMin[dim_x] = (rand() % 3) + (removed->position[dim_x] - 1);
          pMin[dim_y] = (rand() % 3) + (removed->position[dim_y] - 1);
          if(pMin[dim_x] == removed->position[dim_x] && pMin[dim_y] == removed->position[dim_y]) {
            continue;
          }
          if(d->hardness[pMin[dim_y]][pMin[dim_x]] == 0) {
            break;
          }
        }
      } else {
        for(y = removed->position[dim_y] - 1; y < removed->position[dim_y] + 2; y++) {
          for(x = removed->position[dim_x] - 1; x < removed->position[dim_x] + 2; x++) {
            if(d->pc_distance[y][x] < iMin) {
              iMin = d->pc_distance[y][x];
              pMin[dim_x] = x;
              pMin[dim_y] = y;
            }
          }
        }
      }
      mapxy(removed->position[dim_x], removed->position[dim_y]) = removed->prev;
      removed->prev = mapxy(pMin[dim_x], pMin[dim_y]);
      mapxy(pMin[dim_x], pMin[dim_y]) = ter_mon_b;
      removed->position[dim_x] = pMin[dim_x];
      removed->position[dim_y] = pMin[dim_y];
      removed->nextTurn = removed->nextTurn + (1000 / removed->speed);
      removed->hn = heap_insert(&h, removed);
      if(pMin[dim_x] == d->pc.position[dim_x] && pMin[dim_y] == d->pc.position[dim_y]) {
        d->pc.position[dim_x] = 0;
        d->pc.position[dim_y] = 0;
        render_dungeon(d);
        break;
      }
    }
    else if (is == 'c') {
    }
    else if (is == 'd') {
    }
    else if (is == 'e') {
      int erratic = rand() % 2;
      pair_t pMin;
      if (erratic) {
        while(1) {
          pMin[dim_x] = (rand() % 3) + (removed->position[dim_x] - 1);
          pMin[dim_y] = (rand() % 3) + (removed->position[dim_y] - 1);
          if((pMin[dim_x] == removed->position[dim_x] && pMin[dim_y] == removed->position[dim_y]) || pMin[dim_x] <= 0 || pMin[dim_x] >= DUNGEON_X - 1 || pMin[dim_y] <= 0 || pMin[dim_y] >= DUNGEON_Y - 1) {
            continue;
          }
          break;
        }
      } else {
        pMin[dim_x] = removed->position[dim_x];
        pMin[dim_y] = removed->position[dim_y];
        if(removed->position[dim_x] < d->pc.position[dim_x]) {
          ++pMin[dim_x];
        }
        else if (removed->position[dim_x] > d->pc.position[dim_x]) {
          --pMin[dim_x];
        }
      
        if(removed->position[dim_y] < d->pc.position[dim_y]) {
          ++pMin[dim_y];
        }
        else if (removed->position[dim_y] > d->pc.position[dim_y]) {
          --pMin[dim_y];
        }
      }
      
      if(d->hardness[pMin[dim_y]][pMin[dim_x]] > 85) {
        d->hardness[pMin[dim_y]][pMin[dim_x]] = d->hardness[pMin[dim_y]][pMin[dim_x]] - 85;
        dijkstra_tunnel(d);
      }
      else {
        d->hardness[pMin[dim_y]][pMin[dim_x]] = 0;
        if (removed->prev == ter_wall) {
          mapxy(removed->position[dim_x], removed->position[dim_y]) = ter_floor_hall;
          removed->position[dim_x] = pMin[dim_x];
          removed->position[dim_y] = pMin[dim_y];
          removed->prev = mapxy(pMin[dim_x], pMin[dim_y]);
          mapxy(pMin[dim_x], pMin[dim_y]) = ter_mon_e;
          dijkstra(d);
          dijkstra_tunnel(d);
        }
        else {
          mapxy(removed->position[dim_x], removed->position[dim_y]) = removed->prev;
          removed->prev = mapxy(pMin[dim_x], pMin[dim_y]);
          mapxy(pMin[dim_x], pMin[dim_y]) = ter_mon_e;
          removed->position[dim_x] = pMin[dim_x];
          removed->position[dim_y] = pMin[dim_y];
        }
      }
      removed->nextTurn = removed->nextTurn + (1000 / removed->speed);
      removed->hn = heap_insert(&h, removed);
      if(pMin[dim_x] == d->pc.position[dim_x] && pMin[dim_y] == d->pc.position[dim_y]) {
        d->pc.position[dim_x] = 0;
        d->pc.position[dim_y] = 0;
        render_dungeon(d);
        break;
      }
    }
    else if (is == 'f') {
      int erratic = rand() % 2;
      pair_t pMin;
      int iMin = INT_MAX;
      if (erratic) {
        while(1) {
          pMin[dim_x] = (rand() % 3) + (removed->position[dim_x] - 1);
          pMin[dim_y] = (rand() % 3) + (removed->position[dim_y] - 1);
          if((pMin[dim_x] == removed->position[dim_x] && pMin[dim_y] == removed->position[dim_y]) || pMin[dim_x] <= 0 || pMin[dim_x] >= DUNGEON_X - 1 || pMin[dim_y] <= 0 || pMin[dim_y] >= DUNGEON_Y - 1) {
            continue;
          }
          break;
        }
      } else {
        for(y = removed->position[dim_y] - 1; y < removed->position[dim_y] + 2; y++) {
          for(x = removed->position[dim_x] - 1; x < removed->position[dim_x] + 2; x++) {
            if(d->pc_tunnel[y][x] < iMin) {
              iMin = d->pc_tunnel[y][x];
              pMin[dim_x] = x;
              pMin[dim_y] = y;
            }
          }
        }
      }
      
      if(d->hardness[pMin[dim_y]][pMin[dim_x]] > 85) {
        d->hardness[pMin[dim_y]][pMin[dim_x]] = d->hardness[pMin[dim_y]][pMin[dim_x]] - 85;
        dijkstra_tunnel(d);
      }
      else {
        d->hardness[pMin[dim_y]][pMin[dim_x]] = 0;
        if (removed->prev == ter_wall) {
          mapxy(removed->position[dim_x], removed->position[dim_y]) = ter_floor_hall;
          removed->position[dim_x] = pMin[dim_x];
          removed->position[dim_y] = pMin[dim_y];
          removed->prev = mapxy(pMin[dim_x], pMin[dim_y]);
          mapxy(pMin[dim_x], pMin[dim_y]) = ter_mon_f;
          dijkstra(d);
          dijkstra_tunnel(d);
        }
        else {
          mapxy(removed->position[dim_x], removed->position[dim_y]) = removed->prev;
          removed->prev = mapxy(pMin[dim_x], pMin[dim_y]);
          mapxy(pMin[dim_x], pMin[dim_y]) = ter_mon_f;
          removed->position[dim_x] = pMin[dim_x];
          removed->position[dim_y] = pMin[dim_y];
        }
      }
      removed->nextTurn = removed->nextTurn + (1000 / removed->speed);
      removed->hn = heap_insert(&h, removed);
      if(pMin[dim_x] == d->pc.position[dim_x] && pMin[dim_y] == d->pc.position[dim_y]) {
        d->pc.position[dim_x] = 0;
        d->pc.position[dim_y] = 0;
        render_dungeon(d);
        break;
      }
      
    }
    else {
      printf("ERROR ERROR ERROR\n");
    }
    
  }
  //print_heap(&h, NULL);
} 


int main(int argc, char *argv[])
{
  dungeon_t d;
  time_t seed;
  struct timeval tv;
  uint32_t i;
  uint32_t do_load, do_save, do_seed, do_image, do_save_seed, do_save_image;
  uint32_t long_arg;
  char *save_file;
  char *load_file;
  char *pgm_file;

  /* Quiet a false positive from valgrind. */
  memset(&d, 0, sizeof (d));
  
  /* Default behavior: Seed with the time, generate a new dungeon, *
   * and don't write to disk.                                      */
  do_load = do_save = do_image = do_save_seed = do_save_image = 0;
  do_seed = 1;
  save_file = load_file = NULL;

  uint32_t mons = DEFAULT_MONS;

  /* The project spec requires '--load' and '--save'.  It's common  *
   * to have short and long forms of most switches (assuming you    *
   * don't run out of letters).  For now, we've got plenty.  Long   *
   * forms use whole words and take two dashes.  Short forms use an *
    * abbreviation after a single dash.  We'll add '--rand' (to     *
   * specify a random seed), which will take an argument of it's    *
   * own, and we'll add short forms for all three commands, '-l',   *
   * '-s', and '-r', respectively.  We're also going to allow an    *
   * optional argument to load to allow us to load non-default save *
   * files.  No means to save to non-default locations, however.    *
   * And the final switch, '--image', allows me to create a dungeon *
   * from a PGM image, so that I was able to create those more      *
   * interesting test dungeons for you.                             */
 
 if (argc > 1) {
    for (i = 1, long_arg = 0; i < argc; i++, long_arg = 0) {
      if (argv[i][0] == '-') { /* All switches start with a dash */
        if (argv[i][1] == '-') {
          argv[i]++;    /* Make the argument have a single dash so we can */
          long_arg = 1; /* handle long and short args at the same place.  */
        }
        switch (argv[i][1]) {
        case 'r':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-rand")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%lu", &seed) /* Argument is not an integer */) {
            usage(argv[0]);
          }
          do_seed = 0;
          break;
        case 'l':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-load"))) {
            usage(argv[0]);
          }
          do_load = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            load_file = argv[++i];
          }
          break;
        case 's':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-save"))) {
            usage(argv[0]);
          }
          do_save = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll save to it.  If it is "seed", we'll save to    *
	     * <the current seed>.rlg327.  If it is "image", we'll  *
	     * save to <the current image>.rlg327.                  */
	    if (!strcmp(argv[++i], "seed")) {
	      do_save_seed = 1;
	      do_save_image = 0;
	    } else if (!strcmp(argv[i], "image")) {
	      do_save_image = 1;
	      do_save_seed = 0;
	    } else {
	      save_file = argv[i];
	    }
          }
          break;
        case 'i':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-image"))) {
            usage(argv[0]);
          }
          do_image = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            pgm_file = argv[++i];
          }
          break;
	  case 'n':
	     if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-nummon"))) {
	       usage(argv[0]);
	     }
	     if ((argc > i + 1) && argv[i + 1][0] != '-') {
	       mons = atoi(argv[++i]);
	     }
          break;
        default:
          usage(argv[0]);
        }
      } else { /* No dash */
        usage(argv[0]);
      }
    }
  }

  if (do_seed) {
    /* Allows me to generate more than one dungeon *
     * per second, as opposed to time().           */
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }

  printf("Seed is %ld.\n", seed);
  srand(seed);

  init_dungeon(&d);

  if (do_load) {
    read_dungeon(&d, load_file);
  } else if (do_image) {
    read_pgm(&d, pgm_file);
  } else {
    gen_dungeon(&d);
  }

  if (!do_load) {
    /* Set a valid position for the PC */
    d.pc.position[dim_x] = (d.rooms[0].position[dim_x] +
                            (rand() % d.rooms[0].size[dim_x]));
    d.pc.position[dim_y] = (d.rooms[0].position[dim_y] +
                            (rand() % d.rooms[0].size[dim_y]));
  }

  printf("PC is at (y, x): %d, %d\n",
         d.pc.position[dim_y], d.pc.position[dim_x]);

  

  dijkstra(&d);
  dijkstra_tunnel(&d);
  //TODO: parse args for --nummon and add here instead of 10
  generate_mons(&d, mons);
  monster_loop(&d);
  
  //render_dungeon(&d);
  //render_distance_map(&d);
  //render_tunnel_distance_map(&d);
  //render_hardness_map(&d);
  //render_movement_cost_map(&d);

  if (do_save) {
    if (do_save_seed) {
       /* 10 bytes for number, plus dot, extention and null terminator. */
      save_file = malloc(18);
      sprintf(save_file, "%ld.rlg327", seed);
    }
    if (do_save_image) {
      if (!pgm_file) {
	fprintf(stderr, "No image file was loaded.  Using default.\n");
	do_save_image = 0;
      } else {
	/* Extension of 3 characters longer than image extension + null. */
	save_file = malloc(strlen(pgm_file) + 4);
	strcpy(save_file, pgm_file);
	strcpy(strchr(save_file, '.') + 1, "rlg327");
      }
    }
    write_dungeon(&d, save_file);

    if (do_save_seed || do_save_image) {
      free(save_file);
    }
  }

  delete_dungeon(&d);

  return 0;
}
