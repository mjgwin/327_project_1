#ifndef PC_H
# define PC_H

# include <stdint.h>

# include "dims.h"
# include "character.h"
# include "dungeon.h"
# include "object.h"

#define INV_WEAPON 0
#define INV_OFFHAND 1
#define INV_RANGED 2
#define INV_ARMOR 3
#define INV_HELMET 4
#define INV_CLOAK 5
#define INV_GLOVES 6
#define INV_BOOTS 7
#define INV_AMULET 8
#define INV_LIGHT 9
#define INV_RING_1 10
#define INV_RING_2 11
#define EQUIP_SLOTS 12
#define FREE_SLOTS 10

#define DEFAULT_HP 100


class pc : public character {
 public:
  ~pc() {}
  int numItems;
  object *equip_inv[EQUIP_SLOTS];
  object *free_inv[FREE_SLOTS];
  terrain_type known_terrain[DUNGEON_Y][DUNGEON_X];
  uint8_t visible[DUNGEON_Y][DUNGEON_X];
};

void pc_delete(pc *pc);
uint32_t pc_is_alive(dungeon *d);
void config_pc(dungeon *d);
int pc_pickup_object(pc *p, object *o);
void pc_notify_pickup(object *o, int slot);
void pc_equip_item(pc *p, int slot);
uint32_t pc_next_pos(dungeon *d, pair_t dir);
void place_pc(dungeon *d);
uint32_t pc_in_room(dungeon *d, uint32_t room);
void pc_learn_terrain(pc *p, pair_t pos, terrain_type ter);
terrain_type pc_learned_terrain(pc *p, int16_t y, int16_t x);
void pc_init_known_terrain(pc *p);
void pc_observe_terrain(pc *p, dungeon *d);
int32_t is_illuminated(pc *p, int16_t y, int16_t x);
void pc_reset_visibility(pc *p);
void pc_inventory_swap(pc *p, int flag, int slot);
void pc_increase_stats(pc *p, object *o);
void pc_decrease_stats(pc *p, object *o);
#endif
