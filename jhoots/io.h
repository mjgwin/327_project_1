#ifndef IO_H
# define IO_H

# include "character.h"

class dungeon;

void io_init_terminal(void);
void io_reset_terminal(void);
void io_display(dungeon *d);
void io_handle_input(dungeon *d);
void io_queue_message(const char *format, ...);
int io_select_item(dungeon *d);
void io_list_free_inv(dungeon *d);
void io_list_equip_inv(dungeon *d);
const char *io_get_slot_name(dungeon *d, int slot);
int io_select_equip_item(dungeon *d);
void io_remove_equip(dungeon *d);
void io_drop_item(dungeon *d);
void io_expunge_item(dungeon *d);
void io_item_description(dungeon *d);
void io_look_monster(dungeon *d);
void io_mon_info(character *n);
void mod_redisplay_non_terrain(dungeon *d, pair_t cursor);
#endif
