#include <cstdlib>
#include <cstring>
#include <ncurses.h>

#include "dungeon.h"
#include "pc.h"
#include "utils.h"
#include "move.h"
#include "path.h"
#include "io.h"
#include "object.h"

uint32_t pc_is_alive(dungeon *d)
{
  return d->PC->alive;
}

void place_pc(dungeon *d)
{
  d->PC->position[dim_y] = rand_range(d->rooms->position[dim_y],
                                     (d->rooms->position[dim_y] +
                                      d->rooms->size[dim_y] - 1));
  d->PC->position[dim_x] = rand_range(d->rooms->position[dim_x],
                                     (d->rooms->position[dim_x] +
                                      d->rooms->size[dim_x] - 1));

  pc_init_known_terrain(d->PC);
  pc_observe_terrain(d->PC, d);
}

void config_pc(dungeon *d)
{
  static dice pc_dice(0, 1, 4);
  
  d->PC = new pc;

  d->PC->symbol = '@';

  place_pc(d);

  d->PC->speed = PC_SPEED;
  d->PC->alive = 1;
  d->PC->sequence_number = 0;
  d->PC->kills[kill_direct] = d->PC->kills[kill_avenged] = 0;
  d->PC->color.push_back(COLOR_WHITE);
  d->PC->damage = &pc_dice;
  d->PC->name = "Isabella Garcia-Shapiro";
  //Assign defult hp for pc (100 from pc.h)
  d->PC->hp = DEFAULT_HP;
  d->character_map[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC;
  //create inventory and set current items to 0
  d->PC->numItems = 0;
  int i;
  //fills arrays with nullptr so we can loop thru them without crashing
  for(i = 0; i < FREE_SLOTS; i++){
    d->PC->free_inv[i] = nullptr;
  }
  for(i = 0; i < EQUIP_SLOTS; i++){
    d->PC->equip_inv[i] = nullptr;
  }
  
  dijkstra(d);
  dijkstra_tunnel(d);
}
//Places object in first availible space within free inventory
int pc_pickup_object(pc *p, object *o){
  
  if(p->numItems < FREE_SLOTS){
    int i;
    for(i = 0; i < FREE_SLOTS; i++){
      if(p->free_inv[i] != nullptr) continue;
      else{
	 p->free_inv[i] = o;
	 pc_notify_pickup(o, i);
	 p->numItems++;
	 break;
      }
    }
     return 1;
  }else{
    io_queue_message("Inventory full!");
    return 0;
  }
}

//Send message for picked up object and which slot
void pc_notify_pickup(object *o, int slot){

    const int buffer = 50;
    char array1[buffer];
    std::strncpy(array1, "Picked up ", buffer - 1);
    std::string name = o->getName();
    char array2[name.length() + 1];
    strcpy(array2, name.c_str());
    std::strncat(array1, array2, buffer - strlen(array1) - 1);
    char slotNumber[20];
    sprintf(slotNumber, " in slot #%d", slot);
    strcat(array1, slotNumber);
    io_queue_message(array1);
}

//Moves item from free inventory to equip inventory, swapping if needed
void pc_equip_item(pc *p, int slot){
  if(p->free_inv[slot] != nullptr){
    object* obj = p->free_inv[slot];
    object_type_t objType = obj->get_type_t();
    switch(objType){
     case objtype_no_type:
       mvprintw(0,0, "                                                          ");
       mvprintw(0,0,"Cannot equip: item has no type");
       refresh();
       break;
     case objtype_WEAPON:
       pc_inventory_swap(p, INV_WEAPON, slot);
       break;
     case objtype_OFFHAND:
        pc_inventory_swap(p, INV_OFFHAND, slot);
	break;
     case objtype_RANGED:
        pc_inventory_swap(p, INV_RANGED, slot);
	break;
     case objtype_LIGHT:
        pc_inventory_swap(p, INV_LIGHT, slot);
	break;
     case objtype_ARMOR:
        pc_inventory_swap(p, INV_ARMOR, slot);
	break;
     case objtype_HELMET:
        pc_inventory_swap(p, INV_HELMET, slot);
	break;
     case objtype_CLOAK:
        pc_inventory_swap(p, INV_CLOAK, slot);
	break;
     case objtype_GLOVES:
        pc_inventory_swap(p, INV_GLOVES, slot);
	break;
     case objtype_BOOTS:
        pc_inventory_swap(p, INV_BOOTS, slot);
	break;
     case objtype_AMULET:
        pc_inventory_swap(p, INV_AMULET, slot);
	break;
     case objtype_RING:
        pc_inventory_swap(p, INV_RING_1, slot);
	break;
     default:
       mvprintw(0,0, "                                                          ");
       mvprintw(0,0, "Error: item has no type");
       refresh();
    }

    //int buffer = 25;
    //char message[buffer];
    //sprintf(message, "Equipped item from slot %d", slot);
    //mvprintw(0,0,"%s", message);
    //refresh();
  }else{
     mvprintw(0,0, "                                                          ");
     mvprintw(0,0,"Cannot Equip: No item in selected slot");
     refresh();
  }
}

//Helper function to swap items or just equip them
void pc_inventory_swap(pc *p, int flag, int slot){
  if(p->equip_inv[flag]){
    pc_decrease_stats(p, p->equip_inv[flag]);
    pc_increase_stats(p, p->free_inv[slot]);
    object *temp = p->equip_inv[flag];
    p->equip_inv[flag] = p->free_inv[slot];
    p->free_inv[slot] = temp;
    mvprintw(0,0, "                                                          ");
    mvprintw(0,0, "Swapped %s with %s", p->free_inv[slot]->get_name(), p->equip_inv[flag]->get_name());
  }else{
    pc_increase_stats(p, p->free_inv[slot]);
    p->equip_inv[flag] = p->free_inv[slot];
    p->free_inv[slot] = nullptr;
    mvprintw(0,0, "                                                          ");
    mvprintw(0,0, "Equipped %s", p->equip_inv[flag]->get_name());
    p->numItems--;
  }
  refresh();
}

void pc_increase_stats(pc *p, object *o) {
  p->hp = p->hp + o->get_hit();
  p->speed = p->speed + o->get_speed();
  return;
}

void pc_decrease_stats(pc *p, object *o) {
  p->hp = p->hp - o->get_hit();
  p->speed = p->speed - o->get_speed();
  return;
}


uint32_t pc_next_pos(dungeon *d, pair_t dir)
{
  static uint32_t have_seen_corner = 0;
  static uint32_t count = 0;

  dir[dim_y] = dir[dim_x] = 0;

  if (in_corner(d, d->PC)) {
    if (!count) {
      count = 1;
    }
    have_seen_corner = 1;
  }

  /* First, eat anybody standing next to us. */
  if (charxy(d->PC->position[dim_x] - 1, d->PC->position[dim_y] - 1)) {
    dir[dim_y] = -1;
    dir[dim_x] = -1;
  } else if (charxy(d->PC->position[dim_x], d->PC->position[dim_y] - 1)) {
    dir[dim_y] = -1;
  } else if (charxy(d->PC->position[dim_x] + 1, d->PC->position[dim_y] - 1)) {
    dir[dim_y] = -1;
    dir[dim_x] = 1;
  } else if (charxy(d->PC->position[dim_x] - 1, d->PC->position[dim_y])) {
    dir[dim_x] = -1;
  } else if (charxy(d->PC->position[dim_x] + 1, d->PC->position[dim_y])) {
    dir[dim_x] = 1;
  } else if (charxy(d->PC->position[dim_x] - 1, d->PC->position[dim_y] + 1)) {
    dir[dim_y] = 1;
    dir[dim_x] = -1;
  } else if (charxy(d->PC->position[dim_x], d->PC->position[dim_y] + 1)) {
    dir[dim_y] = 1;
  } else if (charxy(d->PC->position[dim_x] + 1, d->PC->position[dim_y] + 1)) {
    dir[dim_y] = 1;
    dir[dim_x] = 1;
  } else if (!have_seen_corner || count < 250) {
    /* Head to a corner and let most of the NPCs kill each other off */
    if (count) {
      count++;
    }
    if (!against_wall(d, d->PC) && ((rand() & 0x111) == 0x111)) {
      dir[dim_x] = (rand() % 3) - 1;
      dir[dim_y] = (rand() % 3) - 1;
    } else {
      dir_nearest_wall(d, d->PC, dir);
    }
  }else {
    /* And after we've been there, let's head toward the center of the map. */
    if (!against_wall(d, d->PC) && ((rand() & 0x111) == 0x111)) {
      dir[dim_x] = (rand() % 3) - 1;
      dir[dim_y] = (rand() % 3) - 1;
    } else {
      dir[dim_x] = ((d->PC->position[dim_x] > DUNGEON_X / 2) ? -1 : 1);
      dir[dim_y] = ((d->PC->position[dim_y] > DUNGEON_Y / 2) ? -1 : 1);
    }
  }

  /* Don't move to an unoccupied location if that places us next to a monster */
  if (!charxy(d->PC->position[dim_x] + dir[dim_x],
              d->PC->position[dim_y] + dir[dim_y]) &&
      ((charxy(d->PC->position[dim_x] + dir[dim_x] - 1,
               d->PC->position[dim_y] + dir[dim_y] - 1) &&
        (charxy(d->PC->position[dim_x] + dir[dim_x] - 1,
                d->PC->position[dim_y] + dir[dim_y] - 1) != d->PC)) ||
       (charxy(d->PC->position[dim_x] + dir[dim_x] - 1,
               d->PC->position[dim_y] + dir[dim_y]) &&
        (charxy(d->PC->position[dim_x] + dir[dim_x] - 1,
                d->PC->position[dim_y] + dir[dim_y]) != d->PC)) ||
       (charxy(d->PC->position[dim_x] + dir[dim_x] - 1,
               d->PC->position[dim_y] + dir[dim_y] + 1) &&
        (charxy(d->PC->position[dim_x] + dir[dim_x] - 1,
                d->PC->position[dim_y] + dir[dim_y] + 1) != d->PC)) ||
       (charxy(d->PC->position[dim_x] + dir[dim_x],
               d->PC->position[dim_y] + dir[dim_y] - 1) &&
        (charxy(d->PC->position[dim_x] + dir[dim_x],
                d->PC->position[dim_y] + dir[dim_y] - 1) != d->PC)) ||
       (charxy(d->PC->position[dim_x] + dir[dim_x],
               d->PC->position[dim_y] + dir[dim_y] + 1) &&
        (charxy(d->PC->position[dim_x] + dir[dim_x],
                d->PC->position[dim_y] + dir[dim_y] + 1) != d->PC)) ||
       (charxy(d->PC->position[dim_x] + dir[dim_x] + 1,
               d->PC->position[dim_y] + dir[dim_y] - 1) &&
        (charxy(d->PC->position[dim_x] + dir[dim_x] + 1,
                d->PC->position[dim_y] + dir[dim_y] - 1) != d->PC)) ||
       (charxy(d->PC->position[dim_x] + dir[dim_x] + 1,
               d->PC->position[dim_y] + dir[dim_y]) &&
        (charxy(d->PC->position[dim_x] + dir[dim_x] + 1,
                d->PC->position[dim_y] + dir[dim_y]) != d->PC)) ||
       (charxy(d->PC->position[dim_x] + dir[dim_x] + 1,
               d->PC->position[dim_y] + dir[dim_y] + 1) &&
        (charxy(d->PC->position[dim_x] + dir[dim_x] + 1,
                d->PC->position[dim_y] + dir[dim_y] + 1) != d->PC)))) {
    dir[dim_x] = dir[dim_y] = 0;
  }

  return 0;
}

uint32_t pc_in_room(dungeon *d, uint32_t room)
{
  if ((room < d->num_rooms)                                     &&
      (d->PC->position[dim_x] >= d->rooms[room].position[dim_x]) &&
      (d->PC->position[dim_x] < (d->rooms[room].position[dim_x] +
                                d->rooms[room].size[dim_x]))    &&
      (d->PC->position[dim_y] >= d->rooms[room].position[dim_y]) &&
      (d->PC->position[dim_y] < (d->rooms[room].position[dim_y] +
                                d->rooms[room].size[dim_y]))) {
    return 1;
  }

  return 0;
}

void pc_learn_terrain(pc *p, pair_t pos, terrain_type ter)
{
  p->known_terrain[pos[dim_y]][pos[dim_x]] = ter;
  p->visible[pos[dim_y]][pos[dim_x]] = 1;
}

void pc_reset_visibility(pc *p)
{
  uint32_t y, x;

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      p->visible[y][x] = 0;
    }
  }
}

terrain_type pc_learned_terrain(pc *p, int16_t y, int16_t x)
{
  if (y < 0 || y >= DUNGEON_Y || x < 0 || x >= DUNGEON_X) {
    io_queue_message("Invalid value to %s: %d, %d", __FUNCTION__, y, x);
  }

  return p->known_terrain[y][x];
}

void pc_init_known_terrain(pc *p)
{
  uint32_t y, x;

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      p->known_terrain[y][x] = ter_unknown;
      p->visible[y][x] = 0;
    }
  }
}

void pc_observe_terrain(pc *p, dungeon *d)
{
  pair_t where;
  int16_t y_min, y_max, x_min, x_max;

  y_min = p->position[dim_y] - PC_VISUAL_RANGE;
  if (y_min < 0) {
    y_min = 0;
  }
  y_max = p->position[dim_y] + PC_VISUAL_RANGE;
  if (y_max > DUNGEON_Y - 1) {
    y_max = DUNGEON_Y - 1;
  }
  x_min = p->position[dim_x] - PC_VISUAL_RANGE;
  if (x_min < 0) {
    x_min = 0;
  }
  x_max = p->position[dim_x] + PC_VISUAL_RANGE;
  if (x_max > DUNGEON_X - 1) {
    x_max = DUNGEON_X - 1;
  }

  for (where[dim_y] = y_min; where[dim_y] <= y_max; where[dim_y]++) {
    where[dim_x] = x_min;
    can_see(d, p->position, where, 1, 1);
    where[dim_x] = x_max;
    can_see(d, p->position, where, 1, 1);
  }
  /* Take one off the x range because we alreay hit the corners above. */
  for (where[dim_x] = x_min - 1; where[dim_x] <= x_max - 1; where[dim_x]++) {
    where[dim_y] = y_min;
    can_see(d, p->position, where, 1, 1);
    where[dim_y] = y_max;
    can_see(d, p->position, where, 1, 1);
  }       
}

int32_t is_illuminated(pc *p, int16_t y, int16_t x)
{
  return p->visible[y][x];
}

void pc_see_object(character *the_pc, object *o)
{
  if (o) {
    o->has_been_seen();
  }
}

