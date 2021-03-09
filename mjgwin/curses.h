#ifndef CURSES_H
# define CURSES_H

# include <ncurses.h>
# include "dungeon.h"
void init_terminal();
void show_screen();
void render_board(dungeon_t *d);
void take_turn(dungeon_t *d,  pair_t next);
int validUpStair(dungeon_t *d);
int validDownStair(dungeon_t *d);
int getStateChange();
void setStateChange(int state);


#endif
