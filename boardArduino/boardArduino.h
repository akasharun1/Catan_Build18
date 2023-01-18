#include <Arduino.h>
#include <stdlib.h>


#define PLAYER0 60

#define REEDSW_X0 11
#define REEDSW_X1 12
#define REEDSW_X2 13
#define RDSW_X_NUM 3

#define REEDSW_Y0 5
#define REEDSW_Y1 6
#define REEDSW_Y2 7
#define REEDSW_Y3 8
#define REEDSW_Y4 9
#define REEDSW_Y5 10
#define RDSW_Y_NUM 6


#define VERTICES_X0 54
#define VERTICES_X1 55
#define VERTICES_X2 56
#define VERTICES_X3 57
#define VERTICES_X4 58
#define VERTICES_X5 59
#define VERTICES_X6 60
#define VERTICES_X7 61
#define VERTICES_X8 62
#define VERTICES_X_NUM 9

#define VERTICES_Y0 63
#define VERTICES_Y1 64
#define VERTICES_Y2 65
#define VERTICES_Y3 66
#define VERTICES_Y4 66
#define VERTICES_Y5 67
#define VERTICES_Y_NUM 6


#define EDGES_X0 30
#define EDGES_X1 32
#define EDGES_X2 34
#define EDGES_X3 36
#define EDGES_X4 38
#define EDGES_X5 40
#define EDGES_X6 42
#define EDGES_X7 44
#define EDGES_X8 46
#define EDGES_X9 48
#define EDGES_X10 50
#define EDGES_X11 52
#define EDGES_X_NUM 12

#define EDGES_Y0 43
#define EDGES_Y1 45
#define EDGES_Y2 47
#define EDGES_Y3 49
#define EDGES_Y4 51
#define EDGES_Y5 53
#define EDGES_Y_NUM 6


#define DiceTX 16
#define DiceRX 17

#define ArduinoRingTX 18
#define ArduinoRingRX 19

#define piDisplayTX 14
#define piDisplayRX 15

enum ControllerColors {
  blueController = 60,
  redController = 61,
  greenController = 62,
  orangeController = 63
};

 enum resource_t{
    desert = 0,
    rock,
    clay,
    wheat,
    wood,
    sheep
};

enum resource_count_t{
    num_desert = 1,
    num_rock = 3,
    num_clay = 3,
    num_wheat = 4,
    num_wood = 4,
    num_sheep = 4
};

enum building_t{
    none = 0,
    blue_settlement,
    blue_city,
    green_settlement,
    green_city,
    red_settlement,
    red_city,
    orange_settlement,
    orange_city
};

struct vertex_t {
    building_t type;
    int ledID;
};

struct edge_t {
    building_t type;
    int ledID1;
    int ledID2;
};

struct tile_t {
    vertex_t vertices[6];
    edge_t edges[6];
    resource_t type;
    int num;
};

struct player_t{
    int woodCount;
    int sheepCount;
    int clayCount;
    int wheatCount;
    int rockCount;
};

struct board_t {
    tile_t tiles[19]; // Tile IDs are assigned by reed switch value
};
