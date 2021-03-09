#include "curses.h"


char board[DUNGEON_Y][DUNGEON_X];
int stateChange = 0;

void init_terminal() {
  initscr();
  raw();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
}

void show_screen(dungeon_t *d){
  clear();
  int x, y;
  
  render_board(d);
  for(y = 0; y < DUNGEON_Y; y++) {
    for(x = 0; x < DUNGEON_X; x++) {
      mvaddch(y, x + 1, board[y][x]);
    }
  }
  refresh();
}

void take_turn(dungeon_t *d, pair_t next) {
  char key;
  
  show_screen(d);
  
  key = getch();
  switch(key) {
    case 'k': //UP
      next[dim_y] = next[dim_y] - 1;
      break;
    case 'j': //DOWN
      next[dim_y] = next[dim_y] + 1;
      break;
    case 'l': //RIGHT
      next[dim_x] = next[dim_x] + 1;
      break;
    case 'h': //LEFT
      next[dim_x] = next[dim_x] - 1;
      break;
    case 'u': //UPPER RIGHT
      next[dim_x] = next[dim_x] + 1;
      next[dim_y] = next[dim_y] - 1;
      break;
    case 'n': //LOWER RIGHT
      next[dim_x] = next[dim_x] + 1;
      next[dim_y] = next[dim_y] + 1;
      break;
    case 'b': //LOWER LEFT
      next[dim_x] = next[dim_x] - 1;
      next[dim_y] = next[dim_y] + 1;
      break;
    case 'y': //UPPER LEFT
      next[dim_x] = next[dim_x] - 1;
      next[dim_y] = next[dim_y] - 1;
      break;
    case '<': //UP STAIRS
      if(validUpStair(d)){
	stateChange = 1;
      }else{
	 mvprintw(23, 1, "Not a valid up stair");
      }
      break;
    case '>': //DOWN STAIRS
      if(validDownStair(d)){
	stateChange = 1;
      }else{
	 mvprintw(23, 1, "Not a valid down stair");
      }
      break;
    case ' ': //STAY IN PLACE
      break;
    default:
      mvprintw(23, 1, "Unkown key: %o", key);
      refresh();
      return;
  }
  show_screen(d);
}

int validUpStair(dungeon_t *d){
  return mappair(d->pc.position) == ter_stairs_up;
}

int validDownStair(dungeon_t *d){
  return mappair(d->pc.position) == ter_stairs_down;
}

void render_board(dungeon_t *d) {
  pair_t p;
  for (p[dim_y] = 0; p[dim_y] < DUNGEON_Y; p[dim_y]++) {
    for (p[dim_x] = 0; p[dim_x] < DUNGEON_X; p[dim_x]++) {
      if (charpair(p)) {
        board[p[dim_y]][p[dim_x]] = charpair(p)->symbol;
      }
      else {
        switch (mappair(p)) {
        case ter_wall:
        case ter_wall_immutable:
          board[p[dim_y]][p[dim_x]] = ' ';
          break;
        case ter_floor:
        case ter_floor_room:
          board[p[dim_y]][p[dim_x]] = '.';
          break;
        case ter_floor_hall:
          board[p[dim_y]][p[dim_x]] = '#';
          break;
        case ter_debug:
          board[p[dim_y]][p[dim_x]] = '*';
          break;
        case ter_stairs_up:
          board[p[dim_y]][p[dim_x]] = '<';
          break;
        case ter_stairs_down:
          board[p[dim_y]][p[dim_x]] = '>';
          break;
        default:
          break;
        }
      }
    }
  }
}

int getStateChange(){
   return stateChange;
}

void setStateChange(int state){
   stateChange = state;
}
