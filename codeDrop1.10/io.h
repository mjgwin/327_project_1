#ifndef IO_H
# define IO_H

# include "character.h"
class dungeon;

void io_init_terminal(void);
void io_reset_terminal(void);
void io_display(dungeon *d);
void io_handle_input(dungeon *d);
void io_queue_message(const char *format, ...);
int io_ranged_attack(dungeon *d);
void mod_redisplay_non_terrain(dungeon *d, pair_t cursor);
void io_range_dam(character *n, dungeon *d);

#endif
