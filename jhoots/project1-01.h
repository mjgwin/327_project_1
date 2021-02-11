#define WORLD_WIDTH 80
#define WORLD_HEIGHT 21
#define MAX_ROOMS 8
#define MIN_ROOMS 6
#define MAX_ROOM_WIDTH 8
#define MIN_ROOM_WIDTH 4
#define MAX_ROOM_HEIGHT 8
#define MIN_ROOM_HEIGHT 3
#define ROOM_SPACE_BUFFER 3
#define EMPTY 0
#define ROOM 1
#define CORRIDOR 2

struct Point{
   int x;
   int y;
};

struct Room{
   int xPos;
   int yPos;
   int xSize;
   int ySize;
};

struct Dungeon{
   int world[WORLD_HEIGHT][WORLD_WIDTH];
   int hardness[WORLD_HEIGHT][WORLD_WIDTH];
   struct Room rooms[MAX_ROOMS];
};

struct Dungeon d;

void print_world(int world[WORLD_HEIGHT][WORLD_WIDTH]);
int in_bounds(int xPos, int yPos, int width, int height);
void place_rooms(int roomCount);
int rooms_overlap(int xPos, int yPos, int xSize, int ySize, int buffer, int numRooms);
struct Point closest_room_to(struct Room rooms, int roomNum);
void place_corridors(int roomNum);
