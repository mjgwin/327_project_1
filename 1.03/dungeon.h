#define WORLD_WIDTH 80
#define WORLD_HEIGHT 21
#define MAX_ROOMS 8
#define MIN_ROOMS 6
#define MAX_ROOM_WIDTH 8
#define MIN_ROOM_WIDTH 5
#define MAX_ROOM_HEIGHT 8
#define MIN_ROOM_HEIGHT 5
#define ROOM_SPACE_BUFFER 3
#define EMPTY 0
#define ROOM 1
#define CORRIDOR 2
#define UP_STAIR 3
#define DOWN_STAIR 4

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
   int boreDist[WORLD_HEIGHT][WORLD_WIDTH];
   int walkDist[WORLD_HEIGHT][WORLD_WIDTH];
   int roomNum;
   struct Room *rooms;
   struct Point upStair;
   struct Point downStair;
};


typedef struct node {
    int prio;
    struct node* next;
    int data[2];

} Node;

struct Dungeon d;
struct Point pc;
int LOAD = 0;
int SAVE = 0;


void print_world(int world[WORLD_HEIGHT][WORLD_WIDTH]);
int in_bounds(int xPos, int yPos, int width, int height);
void place_rooms(int roomCount);
int rooms_overlap(int xPos, int yPos, int xSize, int ySize, int buffer, int numRooms);
struct Point closest_room_to(struct Room rooms, int roomNum);
void place_corridors(int roomNum);
void place_stairs(int numRooms);
void save_file(char *filePath);
void load_file(char *filePath);
void generate_random();
void clean_up();

Node* newNode(int x , int y , int p);
int * peek(Node** head);
void pop(Node** head);
void push(Node** head, int d[2], int p);
int isEmpty(Node** head);
void boringDist();
void walkingDist();
