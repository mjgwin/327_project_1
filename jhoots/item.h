#ifndef ITEM_H
# define ITEM_H

# include "dice.h"
# include "dungeon.h"

class item {
 public:
  char symbol;
  pair_t position;
  int color;
  int weight;
  int hitpoints;
  int damage;
  int attribute;
  int value;
  int dodge;
  int defence;
  int speed;
  int rarity;
  bool artifact;
};

#endif
