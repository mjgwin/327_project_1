#ifndef IO_H
# define IO_H

typedef struct dungeon dungeon_t;

void io_init_terminal(void);
void io_reset_terminal(void);
void io_generate_fog_map(dungeon_t *d);
void io_update_fog_map(dungeon_t *d);
void io_display(dungeon_t *d, uint32_t fogActive);
void io_handle_input(dungeon_t *d);
void io_queue_message(const char *format, ...);
void io_set_fog_status(uint32_t status);
uint32_t io_get_fog_status();
#endif
