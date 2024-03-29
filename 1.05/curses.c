#include "curses.h"
#include <stdlib.h>


char board[DUNGEON_Y][DUNGEON_X];
int stateChange = 0;
int quit = 0;

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
  int lowerBound = 0;
  int upperBound = 24;
  int check = 1;
  int check2 = 1;
  int k;
  //int performedMove = 1;
while(check2) {  
  show_screen(d);
  
  key = getch();
  switch(key) {
    case 'k': //UP
      check2 = 0;
      next[dim_y] = next[dim_y] - 1;
      break;
    case 'j': //DOWN
      check2 = 0;
      next[dim_y] = next[dim_y] + 1;
      break;
    case 'l': //RIGHT
      check2 = 0;
      next[dim_x] = next[dim_x] + 1;
      break;
    case 'h': //LEFT
      check2 = 0;
      next[dim_x] = next[dim_x] - 1;
      break;
    case 'u': //UPPER RIGHT
      check2 = 0;
      next[dim_x] = next[dim_x] + 1;
      next[dim_y] = next[dim_y] - 1;
      break;
    case 'n': //LOWER RIGHT
      check2 = 0;
      next[dim_x] = next[dim_x] + 1;
      next[dim_y] = next[dim_y] + 1;
      break;
    case 'b': //LOWER LEFT
      check2 = 0;
      next[dim_x] = next[dim_x] - 1;
      next[dim_y] = next[dim_y] + 1;
      break;
    case 'y': //UPPER LEFT
      check2 = 0;
      next[dim_x] = next[dim_x] - 1;
      next[dim_y] = next[dim_y] - 1;
      break;
    case '<': //UP STAIRS
      check2 = 0;
      if(validUpStair(d)){
	stateChange = 1;
      }else{
	 mvprintw(23, 1, "Not a valid up stair");
      }
      break;
    case '>': //DOWN STAIRS
      check2 = 0;
      if(validDownStair(d)){
	stateChange = 1;
      }else{
	 mvprintw(23, 1, "Not a valid down stair");
      }
      break;
    case ' ': //STAY IN PLACE
      check2 = 0;
      break;
    case 'm':
      erase();
      if(upperBound > d->num_monsters) {
        upperBound = d->num_monsters;
      }
      check = 1;
      while(check) {
        monster_display(lowerBound, upperBound, d);
        k = getch();
        switch(k) {
          case KEY_DOWN:
            lowerBound += 24;
            upperBound += 24;
            if(upperBound > d->num_monsters) {
              upperBound = d->num_monsters;
              lowerBound = d->num_monsters - 24;
              if(lowerBound < 0) lowerBound = 0;
            }
            break;
          case KEY_UP:
            lowerBound -= 24;
            upperBound -= 24;
            if(lowerBound < 0) { 
              lowerBound = 0;
              upperBound = lowerBound + 24;
            }
            if(upperBound > d->num_monsters) upperBound = d->num_monsters;
            break;
          case 27:
            check = 0;
            break;
          default:
            erase();
            mvprintw(23, 1, "Unkown key: %d", key);
            break;
        }
      }
      break;
    case 'Q':
      d->pc.alive = 0;
      quit = 1;
      check2 = 0;
      break;
    default:
      mvprintw(23, 1, "Unkown key: %d", key);
      refresh();
      break;
  }
}
  show_screen(d);
  return;
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

int getQuit(){
  return quit;
}

void monster_display(int to, int from, dungeon_t *d) {
  int y;
  int *pos;
  char c;
  int i = 0;
  erase();
  for(y = to; y < from; y++) {
    c = mon_char(y, d);
    pos = mon_pos(y, d);
    int uX = d->pc.position[dim_x] - pos[0];
    int uY = d->pc.position[dim_y] - pos[1];
    if(uX < 0 && uY < 0) {
      mvprintw(i++, 1, "Monster #%d: %c, %d East and %d South", y + 1, c, abs(uX), abs(uY));
    }
    else if(uX < 0 && uY > 0) {
      mvprintw(i++, 1, "Monster #%d: %c, %d East and %d North", y + 1, c, abs(uX), abs(uY));
    }
    else if(uX > 0 && uY > 0) {
      mvprintw(i++, 1, "Monster #%d: %c, %d West and %d North", y + 1, c, abs(uX), abs(uY));
    }
    else if(uX > 0 && uY < 0) {
      mvprintw(i++, 1, "Monster #%d: %c, %d West and %d South", y + 1, c, abs(uX), abs(uY));
    }
    else if(uX == 0 && uY > 0) {
      mvprintw(i++, 1, "Monster #%d: %c, %d North", y + 1, c, abs(uY));
    }
    else if(uX == 0 && uY < 0) {
      mvprintw(i++, 1, "Monster #%d: %c, %d South", y + 1, c, abs(uY));
    }
    else if(uY == 0 && uX > 0) {
      mvprintw(i++, 1, "Monster #%d: %c, %d West", y + 1, c, abs(uX));
    }
    else if(uY == 0 && uX < 0) {
      mvprintw(i++, 1, "Monster #%d: %c, %d East", y + 1, c, abs(uX));
    }
    else {
      mvprintw(i++, 1, "Monster #%d: %c, x: %d, y: %d", y + 1, c, uX, uY);
    }
  }
  refresh();
  return;
  
}

int *mon_pos(int order, dungeon_t *d) {
  int *toRet = malloc(sizeof(int) * 2);
  int x = order;
  int y = order;
  int current = 0;
  for(y = 0; y < DUNGEON_Y; y++) {
    for(x = 0; x < DUNGEON_X; x++) {
      if(d->character[y][x] == NULL || d->character[y][x]->symbol == '@') {
        continue;
      }
      if(current == order) {
        toRet[0] = x;
        toRet[1] = y;
      }
      current = current + 1;
    }
  }
  return toRet;
}

char mon_char(int order, dungeon_t *d) {
  char toRet;
  int x, y;
  int current = 0;
  for(y = 0; y < DUNGEON_Y; y++) {
    for(x = 0; x < DUNGEON_X; x++) {
      if(d->character[y][x] == NULL || d->character[y][x]->symbol == '@') {
        continue;
      }
      if(current == order) {
        toRet = d->character[y][x]->symbol;
      }
      current = current + 1;
    }
  }
  return toRet;
}
