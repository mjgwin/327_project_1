#define WORLD_WIDTH 80
#define WORLD_HEIGHT 21
#define MIN_ROOMS 6
#define MAX_ROOMS 15
#define MIN_ROOM_WIDTH 4
#define MIN_ROOM_HEIGHT 3
#define MAX_ROOM_WIDTH  8
#define MAX_ROOM_HEIGHT 6
#define ROOM_SPACE_BUFFER 3
void generate_world();
void place_rooms(int roomCount);
int rooms_overlap(int xPos, int yPos, int width, int height, int buffer,int numRooms);
int in_bounds(int xPos, int yPos, int width, int height);
void fill_edges();
void print_world();
void clean_up();
