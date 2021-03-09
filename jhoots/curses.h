#ifndef CURSES_H
# define CURSES_H

# include <ncurses.h>
# include "dungeon.h"
void init_terminal();
void show_screen();
void render_board(dungeon_t *d);
void take_turn(dungeon_t *d,  pair_t next);



#endif
