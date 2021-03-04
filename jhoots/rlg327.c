#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>

#include "dungeon.h"
#include "path.h"

typedef struct mon {
  heap_node_t *hn;
  pair_t position;
  int32_t nextTurn;
  int32_t speed;
  int32_t prio;
  char c;
  int prev;
} mon_t;


static int32_t mon_cmp(const void *key, const void *with) {
  int32_t cmp = ((mon_t *) key)->nextTurn - ((mon_t *) with)->nextTurn;
  if (cmp == 0) {
    cmp = ((mon_t *) key)->prio - ((mon_t *) with)->prio;
  }
  return cmp;
}

void usage(char *name)
{
  fprintf(stderr,
          "Usage: %s [-r|--rand <seed>] [-l|--load [<file>]]\n"
          "          [-s|--save [<file>]] [-i|--image <pgm file>]\n",
          name);

  exit(-1);
}

void monster_loop(dungeon_t *d){
  heap_t h;
  heap_init(&h, mon_cmp, NULL);
  
  mon_t currMon[3];
  
  currMon[0].position[dim_y] = d->pc.position[dim_y];
  currMon[0].position[dim_x] = d->pc.position[dim_x];
  currMon[0].nextTurn = 0;
  currMon[0].speed = 10;
  currMon[0].prio = 0;
  currMon[0].c = '@';
  
  currMon[0].hn = heap_insert(&h, &currMon[0]);
  
  currMon[1].position[dim_y] = d->rooms[5].position[dim_y];
  currMon[1].position[dim_x] = d->rooms[5].position[dim_x];
  currMon[1].nextTurn = 0;
  currMon[1].speed = 5;
  currMon[1].prio = 1;
  currMon[1].c = 'a';
  currMon[1].prev = mapxy(currMon[1].position[dim_x], currMon[1].position[dim_y]);
  mapxy(currMon[1].position[dim_x], currMon[1].position[dim_y]) = ter_mon_a;
  currMon[1].hn = heap_insert(&h, &currMon[1]);
  
  currMon[2].position[dim_y] = 1;
  currMon[2].position[dim_x] = 1;
  currMon[2].nextTurn = 0;
  currMon[2].speed = 20;
  currMon[2].prio = 2;
  currMon[2].c = '0';
  currMon[2].prev = mapxy(currMon[2].position[dim_x], currMon[2].position[dim_y]);
  mapxy(currMon[2].position[dim_x], currMon[2].position[dim_y]) = ter_mon_0;
  currMon[2].hn = heap_insert(&h, &currMon[2]);
  dijkstra(d);
  dijkstra_tunnel(d);
  render_dungeon(d);

  
  
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
          printf("made it here too\n"); //possibly change to ter_wall_immutable
          mapxy(removed->position[dim_x], removed->position[dim_y]) = ter_floor_hall;
          removed->position[dim_x] = pMin[dim_x];
          removed->position[dim_y] = pMin[dim_y];
          removed->prev = mapxy(pMin[dim_x], pMin[dim_y]);
          mapxy(pMin[dim_x], pMin[dim_y]) = ter_mon_0;
          dijkstra(d);
          dijkstra_tunnel(d);
        }
        else {
          mapxy(removed->position[dim_x], removed->position[dim_y]) = removed->prev;
          removed->prev = mapxy(pMin[dim_x], pMin[dim_y]);
          mapxy(pMin[dim_x], pMin[dim_y]) = ter_mon_0;
          removed->position[dim_x] = pMin[dim_x];
          removed->position[dim_y] = pMin[dim_y];
        }
      }
      removed->nextTurn = removed->nextTurn + (1000 / removed->speed);
      removed->hn = heap_insert(&h, removed);
      if(pMin[dim_x] == d->pc.position[dim_x] && pMin[dim_y] == d->pc.position[dim_y]) {
        d->pc.position[dim_x] = -1;
        d->pc.position[dim_y] = -1;
        render_dungeon(d);
        break;
      }
    }
    else if (is == '1') {
    }
    else if (is == '2') {
    }
    else if (is == '3') {
    }
    else if (is == '4') {
    }
    else if (is == '5') {
    }
    else if (is == '6') {
    }
    else if (is == '7') {
    }
    else if (is == '8') {
    }
    else if (is == '9') {
    }
    else if (is == 'a') {
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
      mapxy(pMin[dim_x], pMin[dim_y]) = ter_mon_a;
        
      removed->position[dim_x] = pMin[dim_x];
      removed->position[dim_y] = pMin[dim_y];
      removed->nextTurn = removed->nextTurn + (1000 / removed->speed);
      removed->hn = heap_insert(&h, removed);
      if(pMin[dim_x] == d->pc.position[dim_x] && pMin[dim_y] == d->pc.position[dim_y]) {
        d->pc.position[dim_x] = -1;
        d->pc.position[dim_y] = -1;
        render_dungeon(d);
        break;
      }
    }
    else if (is == 'b') {
    }
    else if (is == 'c') {
    }
    else if (is == 'd') {
    }
    else if (is == 'e') {
    }
    else if (is == 'f') {
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
