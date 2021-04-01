#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ncurses.h>

#include "dungeon.h"
#include "pc.h"
#include "npc.h"
#include "move.h"
#include "utils.h"
#include "io.h"

const char *victory =
  "\n                                       o\n"
  "                                      $\"\"$o\n"
  "                                     $\"  $$\n"
  "                                      $$$$\n"
  "                                      o \"$o\n"
  "                                     o\"  \"$\n"
  "                oo\"$$$\"  oo$\"$ooo   o$    \"$    ooo\"$oo  $$$\"o\n"
  "   o o o o    oo\"  o\"      \"o    $$o$\"     o o$\"\"  o$      \"$  "
  "\"oo   o o o o\n"
  "   \"$o   \"\"$$$\"   $$         $      \"   o   \"\"    o\"         $"
  "   \"o$$\"    o$$\n"
  "     \"\"o       o  $          $\"       $$$$$       o          $  ooo"
  "     o\"\"\n"
  "        \"o   $$$$o $o       o$        $$$$$\"       $o        \" $$$$"
  "   o\"\n"
  "         \"\"o $$$$o  oo o  o$\"         $$$$$\"        \"o o o o\"  "
  "\"$$$  $\n"
  "           \"\" \"$\"     \"\"\"\"\"            \"\"$\"            \""
  "\"\"      \"\"\" \"\n"
  "            \"oooooooooooooooooooooooooooooooooooooooooooooooooooooo$\n"
  "             \"$$$$\"$$$$\" $$$$$$$\"$$$$$$ \" \"$$$$$\"$$$$$$\"  $$$\""
  "\"$$$$\n"
  "              $$$oo$$$$   $$$$$$o$$$$$$o\" $$$$$$$$$$$$$$ o$$$$o$$$\"\n"
  "              $\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\""
  "\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"$\n"
  "              $\"                                                 \"$\n"
  "              $\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\""
  "$\"$\"$\"$\"$\"$\"$\"$\n"
  "                                   You win!\n\n";

const char *tombstone =
  "\n\n\n\n                /\"\"\"\"\"/\"\"\"\"\"\"\".\n"
  "               /     /         \\             __\n"
  "              /     /           \\            ||\n"
  "             /____ /   Rest in   \\           ||\n"
  "            |     |    Pieces     |          ||\n"
  "            |     |               |          ||\n"
  "            |     |   A. Luser    |          ||\n"
  "            |     |               |          ||\n"
  "            |     |     * *   * * |         _||_\n"
  "            |     |     *\\/* *\\/* |        | TT |\n"
  "            |     |     *_\\_  /   ...\"\"\"\"\"\"| |"
  "| |.\"\"....\"\"\"\"\"\"\"\".\"\"\n"
  "            |     |         \\/..\"\"\"\"\"...\"\"\""
  "\\ || /.\"\"\".......\"\"\"\"...\n"
  "            |     |....\"\"\"\"\"\"\"........\"\"\"\"\""
  "\"^^^^\".......\"\"\"\"\"\"\"\"..\"\n"
  "            |......\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"......"
  "..\"\"\"\"\"....\"\"\"\"\"..\"\"...\"\"\".\n\n"
  "            You're dead.  Better luck in the next life.\n\n\n";

void usage(char *name)
{
  fprintf(stderr,
          "Usage: %s [-r|--rand <seed>] [-l|--load [<file>]]\n"
          "          [-s|--save [<file>]] [-i|--image <pgm file>]\n"
          "          [-n|--nummon <count>]\n",
          name);

  exit(-1);
}

int color_to_int(std::string color) {
  if(color == "BLACK")
    return COLOR_BLACK;
  else if(color == "RED")
    return COLOR_RED;
  else if(color == "GREEN")
    return COLOR_GREEN;
  else if(color == "YELLOW")
    return COLOR_YELLOW;
  else if(color == "BLUE")
    return COLOR_BLUE;
  else if(color == "MAGENTA")
    return COLOR_MAGENTA;
  else if(color == "CYAN")
    return COLOR_CYAN;
  else if(color == "WHITE")
    return COLOR_WHITE;

  std::cout << "Error while parsing color: " + color + " not found" << std::endl;
  return -1;
}

std::string ability_to_string(std::vector<monAbility> abilities){
  std::string abilityString;
  for(monAbility abil: abilities){
    switch(abil){
    case abil_smart:
      abilityString.append("SMART");
      break;
    case abil_tele:
      abilityString.append("TELE");
      break;
    case abil_tunnel:
      abilityString.append("TUNNEL");
      break;
    case abil_erratic:
      abilityString.append("ERRATIC");
      break;
    case abil_pass:
      abilityString.append("PASS");
      break;
    case abil_pickup:
      abilityString.append("PICKUP");
      break;
    case abil_destroy:
      abilityString.append("DESTROY");
      break;
    case abil_uniq:
      abilityString.append("UNIQ");
      break;
    case abil_boss:
      abilityString.append("BOSS");
      break;
    default:
      std::cout << "Error: Failed to convert ability to string" << std::endl;
    }
    abilityString.append(" ");
  }
  return abilityString;
}

std::vector<monAbility> string_to_ability(std::string ability){
  std::vector<monAbility> abilityList;
  std::istringstream iss(ability);
 
  do{
    std::string line;
    iss >> line;
    
    if(line == "SMART")abilityList.push_back(abil_smart);
    else if(line == "TELE")abilityList.push_back(abil_tele);
    else if(line == "TUNNEL")abilityList.push_back(abil_tunnel);
    else if(line == "ERRATIC")abilityList.push_back(abil_erratic);
    else if(line == "PASS")abilityList.push_back(abil_pass);
    else if(line == "PICKUP")abilityList.push_back(abil_pickup);
    else if(line == "DESTROY")abilityList.push_back(abil_destroy);
    else if(line == "UNIQ")abilityList.push_back(abil_uniq);
    else if(line == "BOSS")abilityList.push_back(abil_boss);
    else if(line == "ABIL");
    else if(line == "");
    else std::cout << "Error parsing ability string: " << line << std::endl;
    
  }while(iss);

  return abilityList;
}

std::string int_to_color(int color) {
  if(color == COLOR_BLACK)
    return "BLACK";
  else if(color == COLOR_RED)
    return "RED";
  else if(color == COLOR_GREEN)
    return "GREEN";
  else if(color == COLOR_YELLOW)
    return "YELLOW";
  else if(color == COLOR_BLUE)
    return "BLUE";
  else if(color == COLOR_MAGENTA)
    return "MAGENTA";
  else if(color == COLOR_CYAN)
    return "CYAN";
  else if(color == COLOR_WHITE)
    return "WHITE";

  std::cout << "Error while printing: " << color << " is not a valid int" << std::endl;
  return "";
}
  

dice parse_file_dice(std::string diceStr){
  dice currDice;
  std::string delimiter1 = "+";
  std::string delimiter2 = "d";
	
  std::string token;
  std::string token2;
	
  size_t pos = 0;
  size_t pos_2 = 0;

  while ((pos = diceStr.find(delimiter1)) != std::string::npos) 
  {
	  // extract the first value
	  token = diceStr.substr(0, pos);
	  diceStr.erase(0, pos + delimiter1.length());
	  currDice.base = std::stoi(token);
  }	

  while ((pos_2 = diceStr.find(delimiter2)) != std::string::npos) 
  {
	  token2 = diceStr.substr(0,pos_2);	
	  diceStr.erase(0, pos_2 + delimiter2.length());
	  currDice.dice = std::stoi(token2);
  }

  currDice.sides = std::stoi(diceStr);


  //std::cout << currDice.base << std::endl;
  //std::cout << currDice.dice << std::endl;
  //std::cout << currDice.sides << std::endl;

  return currDice;
}


monsterDesc parse_file_mon(std::string color,std::string desc, std::string name,std::string speed,
			   std::string hp, std::string dam, std::string abil, std::string symb, std::string rrty, int descSize){
  //parse name
  monsterDesc currMon;
  currMon.name = name;

  //parse description
  std::stringstream desc_stream(desc);
  int counter = 0;
  while(desc_stream.good() && counter < descSize){
    std::string line;
    getline(desc_stream, line, '\n');
    int length = line.length();
    if(length > 77) std::cout << "Parsing error: description over 78 characters" << std::endl;
    currMon.desc.push_back(line);
    counter++;
  }
  
  //parse color
  int i = 0;
  std::istringstream s1(color);
  std::string s;
  int colorAmount = 0;
  while(s1 >> s) {
    ++colorAmount;
  }
  currMon.color = (int*) malloc(sizeof(int) * colorAmount);
  currMon.colorSize = colorAmount;
  std::istringstream s2(color);
  while (s2 >> s) {
    currMon.color[i++] = color_to_int(s);
  }
  
  //parse speed
  dice speedDice = parse_file_dice(speed);
  currMon.speed = speedDice;
  
  //TODO: abilities eventually need parsing to int
  currMon.abilities = string_to_ability(abil);

  //parse hp
  dice hpDice = parse_file_dice(hp);
  currMon.hp = hpDice;

  //parse attack
  dice attackDice = parse_file_dice(dam);
  currMon.attackDam = attackDice;

  //parse symbol
  currMon.symbol = symb[0];

  //parse rarity
  currMon.rarity = std::stoi(rrty);

  return currMon;
}

void print_file_mon(monsterDesc mon){
  std::cout << mon.name << std::endl;
  for(int i = 0; i < (int) mon.desc.size(); i++){
     std::cout << mon.desc[i] << std::endl;
  }
  std::cout << mon.symbol << std::endl;

  std::string color;
  for(int i = 0; i < mon.colorSize; i++) {
    color = color + int_to_color(mon.color[i]) + " ";
  }
  std::cout << color << std::endl;
  std::cout << mon.speed.base << "+" << mon.speed.dice << "d" << mon.speed.sides << std::endl;
  std::cout << ability_to_string(mon.abilities) << std::endl;
  std::cout << mon.hp.base << "+" << mon.hp.dice << "d" << mon.hp.sides << std::endl;
  std::cout << mon.attackDam.base << "+" << mon.attackDam.dice << "d" << mon.attackDam.sides << std::endl;
  std::cout << mon.rarity << std::endl;
}




int main(int argc, char *argv[])
{
  dungeon d;
  time_t seed;
  struct timeval tv;
  int32_t i;
  uint32_t do_load, do_save, do_seed, do_image, do_save_seed, do_save_image;
  uint32_t long_arg;
  char *save_file;
  char *load_file;
  char *pgm_file;
  
  /* Quiet a false positive from valgrind. */
  memset(&d, 0, sizeof (d));
  
  /* Default behavior: Seed with the time, generate a new dungeon, *
   * and don't write to disk.                                      */
  do_load = do_save = do_image = do_save_seed = do_save_image = 0;
  do_seed = 1;
  save_file = load_file = NULL;
  d.max_monsters = MAX_MONSTERS;

  /* The project spec requires '--load' and '--save'.  It's common  *
   * to have short and long forms of most switches (assuming you    *
   * don't run out of letters).  For now, we've got plenty.  Long   *
   * forms use whole words and take two dashes.  Short forms use an *
    * abbreviation after a single dash.  We'll add '--rand' (to     *
   * specify a random seed), which will take an argument of it's    *
   * own, and we'll add short forms for all three commands, '-l',   *
   * '-s', and '-r', respectively.  We're also going to allow an    *
   * optional argument to load to allow us to load non-default save *
   * files.  No means to save to non-default locations, however.    *
   * And the final switch, '--image', allows me to create a dungeon *
   * from a PGM image, so that I was able to create those more      *
   * interesting test dungeons for you.                             */
 
 if (argc > 1) {
    for (i = 1, long_arg = 0; i < argc; i++, long_arg = 0) {
      if (argv[i][0] == '-') { /* All switches start with a dash */
        if (argv[i][1] == '-') {
          argv[i]++;    /* Make the argument have a single dash so we can */
          long_arg = 1; /* handle long and short args at the same place.  */
        }
        switch (argv[i][1]) {
        case 'n':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-nummon")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%hu", &d.max_monsters)) {
            usage(argv[0]);
          }
          break;
        case 'r':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-rand")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%lu", &seed) /* Argument is not an integer */) {
            usage(argv[0]);
          }
          do_seed = 0;
          break;
        case 'l':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-load"))) {
            usage(argv[0]);
          }
          do_load = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            load_file = argv[++i];
          }
          break;
        case 's':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-save"))) {
            usage(argv[0]);
          }
          do_save = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll save to it.  If it is "seed", we'll save to    *
	     * <the current seed>.rlg327.  If it is "image", we'll  *
	     * save to <the current image>.rlg327.                  */
	    if (!strcmp(argv[++i], "seed")) {
	      do_save_seed = 1;
	      do_save_image = 0;
	    } else if (!strcmp(argv[i], "image")) {
	      do_save_image = 1;
	      do_save_seed = 0;
	    } else {
	      save_file = argv[i];
	    }
          }
          break;
        case 'i':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-image"))) {
            usage(argv[0]);
          }
          do_image = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            pgm_file = argv[++i];
          }
          break;
        default:
          usage(argv[0]);
        }
      } else { /* No dash */
        usage(argv[0]);
      }
    }
  }

  std::ifstream temp("monster_desc.txt");
  std::string si;
  int size = 0;
  while(temp.good()) {
    getline(temp, si);
    if(si == "BEGIN MONSTER") {
      ++size;
    }
  }
  std::cout << size << std::endl;

  std::string s, color, desc, name, speed, hp, dam, abil, symb, rrty;
  std::ifstream f("monster_desc.txt");

  d.fileMonsters = (monsterDesc*) malloc(sizeof(monsterDesc) * size);

 
  //checks file marker
  getline(f, s);
  if(s != "RLG327 MONSTER DESCRIPTION 1") {
    std::cout << "Wrong file" << std::endl;
    return 0;
  }

  //gets us to the first keyword
  while(1) {
    getline(f, s);
    if(s == "BEGIN MONSTER") {
      break;
    }
  }

  int monIndex = 0;
  //outer loop prints
  while(1) {
    int currMon = 1;
    int descSize = 0;
    //keyword parsing
    while(currMon) {
      f >> s; //gets next keyword
      f.get();
      if(s == "NAME"){
	getline(f, name);
      }
      else if(s == "DESC") {
        while(1) {
	  getline(f, s);
	  if(s == ".") {
	    desc.pop_back();
	    break;
	  }
	  desc = desc + s + "\n";
	  ++descSize;
	}
      }
      else if(s == "COLOR") {
	getline(f, color);
      }
      else if(s == "SPEED") {
	getline(f, speed);
      }
      else if(s == "ABIL") {
	getline(f, abil);
      }
      else if(s == "HP") {
	getline(f, hp);
      }
      else if(s == "DAM") {
	getline(f, dam);
      }
      else if(s == "SYMB") {
	getline(f, symb);
      }
      else if(s == "RRTY") {
	getline(f, rrty);
      }
      else if(s == "END") {
	//set curmon to zero because we are at the end of the mon info
	currMon = 0;
      }
      else {
	std::cout << "File Format incorrect"  << std::endl;
	return -1;
      }
    }
    //check if file is complete
    getline(f, s);
    if(!f.good()) {
      monsterDesc temp = parse_file_mon(color, desc, name, speed, hp, dam, abil, symb, rrty, descSize);
      d.fileMonsters[monIndex++] = temp;
      break;
    }

    //if file is not complete bring us to the next keyword and print values
    getline(f, s);
    if(s == "BEGIN MONSTER") {
      monsterDesc temp = parse_file_mon(color, desc, name, speed, hp, dam, abil, symb, rrty, descSize);
      d.fileMonsters[monIndex++] = temp;
      desc = "";
    }

    
  }

  for(i = 0; i < monIndex; i++) {
    std::cout << "" << std::endl;
    print_file_mon(d.fileMonsters[i]);
  }
  
 
 return 0;

  if (do_seed) {
    /* Allows me to generate more than one dungeon *
     * per second, as opposed to time().           */
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }

  srand(seed);

  io_init_terminal();
  init_dungeon(&d);

  if (do_load) {
    read_dungeon(&d, load_file);
  } else if (do_image) {
    read_pgm(&d, pgm_file);
  } else {
    gen_dungeon(&d);
  }

  /* Ignoring PC position in saved dungeons.  Not a bug. */
  config_pc(&d);
  gen_monsters(&d);

  io_display(&d);
  if (!do_load && !do_image) {
    io_queue_message("Seed is %u.", seed);
  }
  while (pc_is_alive(&d) && dungeon_has_npcs(&d) && !d.quit) {
    do_moves(&d);
  }
  io_display(&d);

  io_reset_terminal();

  if (do_save) {
    if (do_save_seed) {
       /* 10 bytes for number, plus dot, extention and null terminator. */
      save_file = (char *) malloc(18);
      sprintf(save_file, "%ld.rlg327", seed);
    }
    if (do_save_image) {
      if (!pgm_file) {
	fprintf(stderr, "No image file was loaded.  Using default.\n");
	do_save_image = 0;
      } else {
	/* Extension of 3 characters longer than image extension + null. */
	save_file = (char *) malloc(strlen(pgm_file) + 4);
	strcpy(save_file, pgm_file);
	strcpy(strchr(save_file, '.') + 1, "rlg327");
      }
    }
    write_dungeon(&d, save_file);

    if (do_save_seed || do_save_image) {
      free(save_file);
    }
  }

  printf("%s", pc_is_alive(&d) ? victory : tombstone);
  printf("You defended your life in the face of %u deadly beasts.\n"
         "You avenged the cruel and untimely murders of %u "
         "peaceful dungeon residents.\n",
         d.PC->kills[kill_direct], d.PC->kills[kill_avenged]);

  if (pc_is_alive(&d)) {
    /* If the PC is dead, it's in the move heap and will get automatically *
     * deleted when the heap destructs.  In that case, we can't call       *
     * delete_pc(), because it will lead to a double delete.               */
    character_delete(d.PC);
  }

  delete_dungeon(&d);

  return 0;
}
