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
#define VERTICES_Y4 67
#define VERTICES_Y5 68
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

struct lookup_t {
    // Multiplied by two so that lookup will give tile number and index in respective array
    size_t edge_storage[EDGES_Y_NUM][2 * EDGES_X_NUM];
    size_t vertex_storage[VERTICES_Y_NUM][2 * VERTICES_X_NUM];
    size_t reedsw_storage[RDSW_Y_NUM][RDSW_X_NUM];
};

void edge_insert(lookup_t *dict, size_t EDGE_Y, size_t EDGE_X, size_t tile_num, size_t edge_ind) {
    size_t Y, X;

    switch (EDGE_Y) {
        case EDGES_Y0: Y = 0; break;
        case EDGES_Y1: Y = 1; break;
        case EDGES_Y2: Y = 2; break;
        case EDGES_Y3: Y = 3; break;
        case EDGES_Y4: Y = 4; break;
        case EDGES_Y5: Y = 5; break;
        default: Y = -1;
    }
    switch (EDGE_X) {
        case EDGES_X0: X = 0; break;
        case EDGES_X1: X = 1; break;
        case EDGES_X2: X = 2; break;
        case EDGES_X3: X = 3; break;
        case EDGES_X4: X = 4; break;
        case EDGES_X5: X = 5; break;
        case EDGES_X6: X = 6; break;
        case EDGES_X7: X = 7; break;
        case EDGES_X8: X = 8; break;
        case EDGES_X9: X = 9; break;
        case EDGES_X10: X = 10; break;
        case EDGES_X11: X = 11; break;
        default: X = -1;
    }

    if (X == -1 || Y == -1) return;

    dict->edge_storage[Y][2 * X] = tile_num;
    dict->edge_storage[Y][2 * X + 1] = edge_ind;
}

size_t *edge_lookup(lookup_t *dict, size_t EDGE_Y, size_t EDGE_X) {
    size_t Y, X;

    switch (EDGE_Y) {
        case EDGES_Y0: Y = 0; break;
        case EDGES_Y1: Y = 1; break;
        case EDGES_Y2: Y = 2; break;
        case EDGES_Y3: Y = 3; break;
        case EDGES_Y4: Y = 4; break;
        case EDGES_Y5: Y = 5; break;
        default: Y = -1;
    }
    switch (EDGE_X) {
        case EDGES_X0: X = 0; break;
        case EDGES_X1: X = 1; break;
        case EDGES_X2: X = 2; break;
        case EDGES_X3: X = 3; break;
        case EDGES_X4: X = 4; break;
        case EDGES_X5: X = 5; break;
        case EDGES_X6: X = 6; break;
        case EDGES_X7: X = 7; break;
        case EDGES_X8: X = 8; break;
        case EDGES_X9: X = 9; break;
        case EDGES_X10: X = 10; break;
        case EDGES_X11: X = 11; break;
        default: X = -1;
    }

    if (X == -1 || Y == -1) return NULL;

    return &(dict->edge_storage[Y][2 * X]);
}

void vertex_insert(lookup_t *dict, size_t VERTEX_Y, size_t VERTEX_X, size_t tile_num, size_t edge_ind) {
    size_t Y, X;

    switch (VERTEX_Y) {
        case VERTICES_Y0: Y = 0; break;
        case VERTICES_Y1: Y = 1; break;
        case VERTICES_Y2: Y = 2; break;
        case VERTICES_Y3: Y = 3; break;
        case VERTICES_Y4: Y = 4; break;
        case VERTICES_Y5: Y = 5; break;
        default: Y = -1;
    }
    switch (VERTEX_X) {
        case VERTICES_X0: X = 0; break;
        case VERTICES_X1: X = 1; break;
        case VERTICES_X2: X = 2; break;
        case VERTICES_X3: X = 3; break;
        case VERTICES_X4: X = 4; break;
        case VERTICES_X5: X = 5; break;
        case VERTICES_X6: X = 6; break;
        case VERTICES_X7: X = 7; break;
        case VERTICES_X8: X = 8; break;
        default: X = -1;
    }

    if (X == -1 || Y == -1) return;

    dict->vertex_storage[Y][2 * X] = tile_num;
    dict->vertex_storage[Y][2 * X + 1] = edge_ind;
}

size_t *vertex_lookup(lookup_t *dict, size_t VERTEX_Y, size_t VERTEX_X) {
size_t Y, X;

    switch (VERTEX_Y) {
        case VERTICES_Y0: Y = 0; break;
        case VERTICES_Y1: Y = 1; break;
        case VERTICES_Y2: Y = 2; break;
        case VERTICES_Y3: Y = 3; break;
        case VERTICES_Y4: Y = 4; break;
        case VERTICES_Y5: Y = 5; break;
        default: Y = -1;
    }
    switch (VERTEX_X) {
        case VERTICES_X0: X = 0; break;
        case VERTICES_X1: X = 1; break;
        case VERTICES_X2: X = 2; break;
        case VERTICES_X3: X = 3; break;
        case VERTICES_X4: X = 4; break;
        case VERTICES_X5: X = 5; break;
        case VERTICES_X6: X = 6; break;
        case VERTICES_X7: X = 7; break;
        case VERTICES_X8: X = 8; break;
        default: X = -1;
    }

    if (X == -1 || Y == -1) return NULL;

    return &(dict->vertex_storage[Y][2 * X]);
}


void reedsw_insert(lookup_t *dict, size_t REEDSW_Y, size_t REEDSW_X, size_t tile_num) {
    size_t Y, X;

    switch (REEDSW_Y) {
        case REEDSW_Y0: Y = 0; break;
        case REEDSW_Y1: Y = 1; break;
        case REEDSW_Y2: Y = 2; break;
        case REEDSW_Y3: Y = 3; break;
        case REEDSW_Y4: Y = 4; break;
        case REEDSW_Y5: Y = 5; break;
        default: Y = -1;
    }
    switch (REEDSW_X) {
        case REEDSW_X0: X = 0; break;
        case REEDSW_X1: X = 1; break;
        case REEDSW_X2: X = 2; break;
        default: X = -1;
    }

    if (X == -1 || Y == -1) return;

    dict->reedsw_storage[Y][2 * X] = tile_num;
}

size_t reedsw_lookup(lookup_t *dict, size_t REEDSW_Y, size_t REEDSW_X) {
    size_t Y, X;

    switch (REEDSW_Y) {
        case REEDSW_Y0: Y = 0; break;
        case REEDSW_Y1: Y = 1; break;
        case REEDSW_Y2: Y = 2; break;
        case REEDSW_Y3: Y = 3; break;
        case REEDSW_Y4: Y = 4; break;
        case REEDSW_Y5: Y = 5; break;
        default: Y = -1;
    }
    switch (REEDSW_X) {
        case REEDSW_X0: X = 0; break;
        case REEDSW_X1: X = 1; break;
        case REEDSW_X2: X = 2; break;
        default: X = -1;
    }

    if (X == -1 || Y == -1) return NULL;

    return (dict->reedsw_storage[Y][X]);
}

// EXAMPLES FOR INSERTING INTO DICT
// reedsw_insert(dict, REEDSW_Y0, REEDSW_X0, TILE VALUE HERE);
// reedsw_insert(dict, REEDSW_Y5, REEDSW_X2, TILE VALUE HERE);

// edge_insert(dict, EDGES_Y0, EDGES_X0, TILE VALUE HERE, EDGE INDEX HERE);
// edge_insert(dict, EDGES_Y5, EDGES_X11, TILE VALUE HERE, EDGE INDEX HERE);

// vertex_insert(dict, VERTICES_Y0, VERTICES_X0, TILE VALUE HERE, EDGE INDEX HERE);
// vertex_insert(dict, VERTICES_Y5, VERTICES_X8, TILE VALUE HERE, EDGE INDEX HERE);
void initDict(lookup_t *dict) {
//     edge_insert(dict, EDGES_Y0, EDGES_X0, , );
//     edge_insert(dict, EDGES_Y0, EDGES_X1, , );
//     edge_insert(dict, EDGES_Y0, EDGES_X2, , );
//     edge_insert(dict, EDGES_Y0, EDGES_X3, , );
//     edge_insert(dict, EDGES_Y0, EDGES_X4, , );
//     edge_insert(dict, EDGES_Y0, EDGES_X5, , );
//     edge_insert(dict, EDGES_Y0, EDGES_X6, , );
//     edge_insert(dict, EDGES_Y0, EDGES_X7, , );
//     edge_insert(dict, EDGES_Y0, EDGES_X8, , );
//     edge_insert(dict, EDGES_Y0, EDGES_X9, , );
//     edge_insert(dict, EDGES_Y0, EDGES_X10, , );
//     edge_insert(dict, EDGES_Y0, EDGES_X11, , );

//     edge_insert(dict, EDGES_Y1, EDGES_X0, , );
//     edge_insert(dict, EDGES_Y1, EDGES_X1, , );
//     edge_insert(dict, EDGES_Y1, EDGES_X2, , );
//     edge_insert(dict, EDGES_Y1, EDGES_X3, , );
//     edge_insert(dict, EDGES_Y1, EDGES_X4, , );
//     edge_insert(dict, EDGES_Y1, EDGES_X5, , );
//     edge_insert(dict, EDGES_Y1, EDGES_X6, , );
//     edge_insert(dict, EDGES_Y1, EDGES_X7, , );
//     edge_insert(dict, EDGES_Y1, EDGES_X8, , );
//     edge_insert(dict, EDGES_Y1, EDGES_X9, , );
//     edge_insert(dict, EDGES_Y1, EDGES_X10, , );
//     edge_insert(dict, EDGES_Y1, EDGES_X11, , );

//     edge_insert(dict, EDGES_Y2, EDGES_X0, , );
//     edge_insert(dict, EDGES_Y2, EDGES_X1, , );
//     edge_insert(dict, EDGES_Y2, EDGES_X2, , );
//     edge_insert(dict, EDGES_Y2, EDGES_X3, , );
//     edge_insert(dict, EDGES_Y2, EDGES_X4, , );
//     edge_insert(dict, EDGES_Y2, EDGES_X5, , );
//     edge_insert(dict, EDGES_Y2, EDGES_X6, , );
//     edge_insert(dict, EDGES_Y2, EDGES_X7, , );
//     edge_insert(dict, EDGES_Y2, EDGES_X8, , );
//     edge_insert(dict, EDGES_Y2, EDGES_X9, , );
//     edge_insert(dict, EDGES_Y2, EDGES_X10, , );
//     edge_insert(dict, EDGES_Y2, EDGES_X11, , );

//     edge_insert(dict, EDGES_Y3, EDGES_X0, , );
//     edge_insert(dict, EDGES_Y3, EDGES_X1, , );
//     edge_insert(dict, EDGES_Y3, EDGES_X2, , );
//     edge_insert(dict, EDGES_Y3, EDGES_X3, , );
//     edge_insert(dict, EDGES_Y3, EDGES_X4, , );
//     edge_insert(dict, EDGES_Y3, EDGES_X5, , );
//     edge_insert(dict, EDGES_Y3, EDGES_X6, , );
//     edge_insert(dict, EDGES_Y3, EDGES_X7, , );
//     edge_insert(dict, EDGES_Y3, EDGES_X8, , );
//     edge_insert(dict, EDGES_Y3, EDGES_X9, , );
//     edge_insert(dict, EDGES_Y3, EDGES_X10, , );
//     edge_insert(dict, EDGES_Y3, EDGES_X11, , );

//     edge_insert(dict, EDGES_Y4, EDGES_X0, , );
//     edge_insert(dict, EDGES_Y4, EDGES_X1, , );
//     edge_insert(dict, EDGES_Y4, EDGES_X2, , );
//     edge_insert(dict, EDGES_Y4, EDGES_X3, , );
//     edge_insert(dict, EDGES_Y4, EDGES_X4, , );
//     edge_insert(dict, EDGES_Y4, EDGES_X5, , );
//     edge_insert(dict, EDGES_Y4, EDGES_X6, , );
//     edge_insert(dict, EDGES_Y4, EDGES_X7, , );
//     edge_insert(dict, EDGES_Y4, EDGES_X8, , );
//     edge_insert(dict, EDGES_Y4, EDGES_X9, , );
//     edge_insert(dict, EDGES_Y4, EDGES_X10, , );
//     edge_insert(dict, EDGES_Y4, EDGES_X11, , );

//     edge_insert(dict, EDGES_Y5, EDGES_X0, , );
//     edge_insert(dict, EDGES_Y5, EDGES_X1, , );
//     edge_insert(dict, EDGES_Y5, EDGES_X2, , );
//     edge_insert(dict, EDGES_Y5, EDGES_X3, , );
//     edge_insert(dict, EDGES_Y5, EDGES_X4, , );
//     edge_insert(dict, EDGES_Y5, EDGES_X5, , );
//     edge_insert(dict, EDGES_Y5, EDGES_X6, , );
//     edge_insert(dict, EDGES_Y5, EDGES_X7, , );
//     edge_insert(dict, EDGES_Y5, EDGES_X8, , );
//     edge_insert(dict, EDGES_Y5, EDGES_X9, , );
//     edge_insert(dict, EDGES_Y5, EDGES_X10, , );
//     edge_insert(dict, EDGES_Y5, EDGES_X11, , );


//     vertex_insert(dict, VERTICES_Y0, VERTICES_X0, , );
//     vertex_insert(dict, VERTICES_Y0, VERTICES_X1, , );
//     vertex_insert(dict, VERTICES_Y0, VERTICES_X2, , );
//     vertex_insert(dict, VERTICES_Y0, VERTICES_X3, , );
//     vertex_insert(dict, VERTICES_Y0, VERTICES_X4, , );
//     vertex_insert(dict, VERTICES_Y0, VERTICES_X5, , );
//     vertex_insert(dict, VERTICES_Y0, VERTICES_X6, , );
//     vertex_insert(dict, VERTICES_Y0, VERTICES_X7, , );
//     vertex_insert(dict, VERTICES_Y0, VERTICES_X8, , );

//     vertex_insert(dict, VERTICES_Y1, VERTICES_X0, , );
//     vertex_insert(dict, VERTICES_Y1, VERTICES_X1, , );
//     vertex_insert(dict, VERTICES_Y1, VERTICES_X2, , );
//     vertex_insert(dict, VERTICES_Y1, VERTICES_X3, , );
//     vertex_insert(dict, VERTICES_Y1, VERTICES_X4, , );
//     vertex_insert(dict, VERTICES_Y1, VERTICES_X5, , );
//     vertex_insert(dict, VERTICES_Y1, VERTICES_X6, , );
//     vertex_insert(dict, VERTICES_Y1, VERTICES_X7, , );
//     vertex_insert(dict, VERTICES_Y1, VERTICES_X8, , );

//     vertex_insert(dict, VERTICES_Y2, VERTICES_X0, , );
//     vertex_insert(dict, VERTICES_Y2, VERTICES_X1, , );
//     vertex_insert(dict, VERTICES_Y2, VERTICES_X2, , );
//     vertex_insert(dict, VERTICES_Y2, VERTICES_X3, , );
//     vertex_insert(dict, VERTICES_Y2, VERTICES_X4, , );
//     vertex_insert(dict, VERTICES_Y2, VERTICES_X5, , );
//     vertex_insert(dict, VERTICES_Y2, VERTICES_X6, , );
//     vertex_insert(dict, VERTICES_Y2, VERTICES_X7, , );
//     vertex_insert(dict, VERTICES_Y2, VERTICES_X8, , );

//     vertex_insert(dict, VERTICES_Y3, VERTICES_X0, , );
//     vertex_insert(dict, VERTICES_Y3, VERTICES_X1, , );
//     vertex_insert(dict, VERTICES_Y3, VERTICES_X2, , );
//     vertex_insert(dict, VERTICES_Y3, VERTICES_X3, , );
//     vertex_insert(dict, VERTICES_Y3, VERTICES_X4, , );
//     vertex_insert(dict, VERTICES_Y3, VERTICES_X5, , );
//     vertex_insert(dict, VERTICES_Y3, VERTICES_X6, , );
//     vertex_insert(dict, VERTICES_Y3, VERTICES_X7, , );
//     vertex_insert(dict, VERTICES_Y3, VERTICES_X8, , );

//     vertex_insert(dict, VERTICES_Y4, VERTICES_X0, , );
//     vertex_insert(dict, VERTICES_Y4, VERTICES_X1, , );
//     vertex_insert(dict, VERTICES_Y4, VERTICES_X2, , );
//     vertex_insert(dict, VERTICES_Y4, VERTICES_X3, , );
//     vertex_insert(dict, VERTICES_Y4, VERTICES_X4, , );
//     vertex_insert(dict, VERTICES_Y4, VERTICES_X5, , );
//     vertex_insert(dict, VERTICES_Y4, VERTICES_X6, , );
//     vertex_insert(dict, VERTICES_Y4, VERTICES_X7, , );
//     vertex_insert(dict, VERTICES_Y4, VERTICES_X8, , );

//     vertex_insert(dict, VERTICES_Y5, VERTICES_X0, , );
//     vertex_insert(dict, VERTICES_Y5, VERTICES_X1, , );
//     vertex_insert(dict, VERTICES_Y5, VERTICES_X2, , );
//     vertex_insert(dict, VERTICES_Y5, VERTICES_X3, , );
//     vertex_insert(dict, VERTICES_Y5, VERTICES_X4, , );
//     vertex_insert(dict, VERTICES_Y5, VERTICES_X5, , );
//     vertex_insert(dict, VERTICES_Y5, VERTICES_X6, , );
//     vertex_insert(dict, VERTICES_Y5, VERTICES_X7, , );
//     vertex_insert(dict, VERTICES_Y5, VERTICES_X8, , );


    reedsw_insert(dict, REEDSW_X0, REEDSW_Y0, 0);
    reedsw_insert(dict, REEDSW_X0, REEDSW_Y1, 1);
    reedsw_insert(dict, REEDSW_X0, REEDSW_Y2, 2);
    reedsw_insert(dict, REEDSW_X0, REEDSW_Y3, 3);
    reedsw_insert(dict, REEDSW_X0, REEDSW_Y4, 4);
    reedsw_insert(dict, REEDSW_X0, REEDSW_Y5, 5);
    reedsw_insert(dict, REEDSW_X1, REEDSW_Y0, 6);
    reedsw_insert(dict, REEDSW_X1, REEDSW_Y1, 7);
    reedsw_insert(dict, REEDSW_X1, REEDSW_Y2, 8);
    reedsw_insert(dict, REEDSW_X1, REEDSW_Y3, 9);
    reedsw_insert(dict, REEDSW_X1, REEDSW_Y4, 10);
    reedsw_insert(dict, REEDSW_X1, REEDSW_Y5, 11);
    reedsw_insert(dict, REEDSW_X2, REEDSW_Y0, 12);
    reedsw_insert(dict, REEDSW_X2, REEDSW_Y1, 13);
    reedsw_insert(dict, REEDSW_X2, REEDSW_Y2, 14);
    reedsw_insert(dict, REEDSW_X2, REEDSW_Y3, 15);
    reedsw_insert(dict, REEDSW_X2, REEDSW_Y4, 16);
    reedsw_insert(dict, REEDSW_X2, REEDSW_Y5, 17);
}
