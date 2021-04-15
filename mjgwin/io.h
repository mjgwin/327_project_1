#ifndef IO_H
# define IO_H

class dungeon;

void io_init_terminal(void);
void io_reset_terminal(void);
void io_display(dungeon *d);
void io_handle_input(dungeon *d);
void io_queue_message(const char *format, ...);
int io_select_item();
void io_list_free_inv(dungeon *d);
void io_list_equip_inv(dungeon *d);
const char *io_get_slot_name(dungeon *d, int slot);
#endif
