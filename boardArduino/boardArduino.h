#include <Arduino.h>
#include <stdlib.h>
#include <Adafruit_NeoPixel.h>

#define NEOPIN 22
#define NUMPIXELS 6 * 33




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

struct playerOnVertex_t {
    int itemCount;
    int playerIndex;
};

struct lookup_t {
    // Multiplied by two so that lookup will give tile number and index in respective array
    // Multiplied by three because vertices can be on up to 3 tiles
    // Multiplied by 2 because edges can be on up to 2 tiles
    size_t edge_storage[EDGES_Y_NUM][2 * 2 * EDGES_X_NUM];
    size_t vertex_storage[VERTICES_Y_NUM][3 * 2 * VERTICES_X_NUM];
    size_t reedsw_storage[RDSW_Y_NUM][RDSW_X_NUM];
};

void edge_insert(lookup_t *dict, board_t *board, size_t EDGE_Y, size_t EDGE_X, size_t tile_num1, size_t edge_ind1, size_t tile_num2, size_t edge_ind2) {
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

    int ledID1, ledID2;

    switch (X) {
        case 0: {
            ledID1 = 33 * Y + 18;
            ledID2 = 33 * Y + 19;
            break;
        }
        case 1 ... 9: {
            ledID1 = 33 * Y + 2 * (X - 1);
            ledID2 = 33 * Y + 2 * (X - 1) + 1;
            break;
        }
        case 10: {
            ledID1 = 33 * Y + 22;
            ledID2 = 33 * Y + 23;
            break;
        }
        case 11: {
            ledID1 = 33 * Y + 20;
            ledID2 = 33 * Y + 21;
            break;
        }
        default: Serial.println("LED NOT RECOGNIZED");
    }

    board->tiles[tile_num1].edges[edge_ind1].ledID1 = ledID1;
    board->tiles[tile_num1].edges[edge_ind1].ledID2 = ledID2;
    if (tile_num2 != -1) {
        board->tiles[tile_num2].edges[edge_ind2].ledID1 = ledID1;
        board->tiles[tile_num2].edges[edge_ind2].ledID2 = ledID2;
    }

    dict->edge_storage[Y][4 * X] = tile_num1;
    dict->edge_storage[Y][4 * X + 1] = edge_ind1;

    dict->edge_storage[Y][4 * X + 2] = tile_num2;
    dict->edge_storage[Y][4 * X + 3] = edge_ind2;
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

    return &(dict->edge_storage[Y][4 * X]);
}

void vertex_insert(lookup_t *dict, board_t *board, size_t VERTEX_Y, size_t VERTEX_X, size_t tile_num1, size_t vert_ind1, size_t tile_num2, size_t vert_ind2, size_t tile_num3, size_t vert_ind3) {
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

    int ledID = 33 * Y + EDGES_X_NUM * 2 + X;
    board->tiles[tile_num1].vertices[vert_ind1].ledID = ledID;
    if (tile_num2 != -1) {
        board->tiles[tile_num2].vertices[vert_ind2].ledID = ledID;
    }
    if (tile_num3 != -1) {
        board->tiles[tile_num3].vertices[vert_ind3].ledID = ledID;
    }

    dict->vertex_storage[Y][6 * X] = tile_num1;
    dict->vertex_storage[Y][6 * X + 1] = vert_ind1;

    dict->vertex_storage[Y][6 * X + 2] = tile_num2;
    dict->vertex_storage[Y][6 * X + 3] = vert_ind2;

    dict->vertex_storage[Y][6 * X + 4] = tile_num3;
    dict->vertex_storage[Y][6 * X + 5] = vert_ind3;
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

    return &(dict->vertex_storage[Y][6 * X]);
}


void reedsw_insert(lookup_t *dict, size_t REEDSW_X, size_t REEDSW_Y, size_t tile_num) {
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

    dict->reedsw_storage[Y][X] = tile_num;
}

size_t reedsw_lookup(lookup_t *dict, size_t REEDSW_X, size_t REEDSW_Y) {
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

void edge_slice_init(size_t slice_number, lookup_t *dict, board_t *board) {
    size_t offset = 3 * slice_number;
    size_t tile0 = offset;
    size_t tile1 = offset + 1;
    size_t tile2 = offset + 2;

    edge_insert(dict, board, EDGES_Y0 + 2 * slice_number, EDGES_X0, tile0, 1, tile1, 4);
    edge_insert(dict, board,EDGES_Y0 + 2 * slice_number, EDGES_X1, tile1, 1, -1, -1);
    edge_insert(dict, board,EDGES_Y0 + 2 * slice_number, EDGES_X2, tile1, 0, -1, -1);
    edge_insert(dict, board,EDGES_Y0 + 2 * slice_number, EDGES_X3, tile2, 1, -1, -1);
    edge_insert(dict, board,EDGES_Y0 + 2 * slice_number, EDGES_X4, tile2, 0, -1, -1);
    edge_insert(dict, board,EDGES_Y0 + 2 * slice_number, EDGES_X5, tile2, 5, -1, -1);
    edge_insert(dict, board,EDGES_Y0 + 2 * slice_number, EDGES_X6, tile2, 4, (tile2 + 2) % 18, 2);
    edge_insert(dict, board,EDGES_Y0 + 2 * slice_number, EDGES_X7, tile0, 5, (tile2 + 2) % 18, 3);
    edge_insert(dict, board,EDGES_Y0 + 2 * slice_number, EDGES_X8, tile0, 4, (tile0 + 3) % 18, 2);
    edge_insert(dict, board,EDGES_Y0 + 2 * slice_number, EDGES_X9, tile0, 3, 18, slice_number);
    edge_insert(dict, board,EDGES_Y0 + 2 * slice_number, EDGES_X10, tile1, 5, tile2, 2);
    edge_insert(dict, board,EDGES_Y0 + 2 * slice_number, EDGES_X11, tile0, 0, tile2, 3);
}

void vertex_slice_init(size_t slice_number, lookup_t *dict, board_t *board) {
    size_t offset = 3 * slice_number;
    size_t tile0 = offset;
    size_t tile1 = offset + 1;
    size_t tile2 = offset + 2;

    vertex_insert(dict, board, VERTICES_Y0 + slice_number, VERTICES_X0, tile1, 0, -1, -1, -1, -1);
    vertex_insert(dict, board,VERTICES_Y0 + slice_number, VERTICES_X1, tile1, 5, tile2, 1, -1, -1);
    vertex_insert(dict, board,VERTICES_Y0 + slice_number, VERTICES_X2, tile2, 0, -1, -1, -1, -1);
    vertex_insert(dict, board,VERTICES_Y0 + slice_number, VERTICES_X3, tile2, 5, -1, -1, -1, -1);
    vertex_insert(dict, board,VERTICES_Y0 + slice_number, VERTICES_X4, tile2, 4, (tile2 + 2) % 18, 1, -1, -1);
    vertex_insert(dict, board,VERTICES_Y0 + slice_number, VERTICES_X5, tile2, 3, tile0, 5, (tile2 + 2) % 18, 2);
    vertex_insert(dict, board,VERTICES_Y0 + slice_number, VERTICES_X6, tile0, 4, (tile2 + 1) % 18, 1, (tile2 + 2) % 18, 3);
    vertex_insert(dict, board,VERTICES_Y0 + slice_number, VERTICES_X7, tile0, 3, (tile2 + 1) % 18, 2, 18, slice_number);
    vertex_insert(dict, board,VERTICES_Y0 + slice_number, VERTICES_X8, tile0, 0, tile1, 4, tile2, 2);
}


// EXAMPLES FOR INSERTING INTO DICT
// reedsw_insert(dict, REEDSW_Y0, REEDSW_X0, TILE VALUE HERE);
// reedsw_insert(dict, REEDSW_Y5, REEDSW_X2, TILE VALUE HERE);

// edge_insert(dict, EDGES_Y0, EDGES_X0, TILE VALUE HERE, EDGE INDEX HERE);
// edge_insert(dict, EDGES_Y5, EDGES_X11, TILE VALUE HERE, EDGE INDEX HERE);

// vertex_insert(dict, VERTICES_Y0, VERTICES_X0, TILE VALUE HERE, EDGE INDEX HERE);
// vertex_insert(dict, VERTICES_Y5, VERTICES_X8, TILE VALUE HERE, EDGE INDEX HERE);
void initDict(lookup_t *dict, board_t *board) {
    for (int i = 0; i < 6; i++) {
        edge_slice_init(i, dict, board);
        vertex_slice_init(i, dict, board);
    }

    reedsw_insert(dict, REEDSW_X0, REEDSW_Y0, 0);
    reedsw_insert(dict, REEDSW_X1, REEDSW_Y0, 1);
    reedsw_insert(dict, REEDSW_X2, REEDSW_Y0, 2);

    reedsw_insert(dict, REEDSW_X0, REEDSW_Y1, 3);
    reedsw_insert(dict, REEDSW_X1, REEDSW_Y1, 4);
    reedsw_insert(dict, REEDSW_X2, REEDSW_Y1, 5);
    
    reedsw_insert(dict, REEDSW_X0, REEDSW_Y2, 6);
    reedsw_insert(dict, REEDSW_X1, REEDSW_Y2, 7);
    reedsw_insert(dict, REEDSW_X2, REEDSW_Y2, 8);

    reedsw_insert(dict, REEDSW_X0, REEDSW_Y3, 9);
    reedsw_insert(dict, REEDSW_X1, REEDSW_Y3, 10);
    reedsw_insert(dict, REEDSW_X2, REEDSW_Y3, 11);

    reedsw_insert(dict, REEDSW_X0, REEDSW_Y4, 12);
    reedsw_insert(dict, REEDSW_X1, REEDSW_Y4, 13);
    reedsw_insert(dict, REEDSW_X2, REEDSW_Y4, 14);
    
    reedsw_insert(dict, REEDSW_X0, REEDSW_Y5, 15);
    reedsw_insert(dict, REEDSW_X1, REEDSW_Y5, 16);
    reedsw_insert(dict, REEDSW_X2, REEDSW_Y5, 17);



    // edge_insert(dict, EDGES_Y1, EDGES_X0, 0 + 3 * slice_number, 1, 1 + 3 * slice_number, 4);
    // edge_insert(dict, EDGES_Y1, EDGES_X1, 1, 1, -1, -1);
    // edge_insert(dict, EDGES_Y1, EDGES_X2, 1, 0, -1, -1);
    // edge_insert(dict, EDGES_Y1, EDGES_X3, 2, 1, -1, -1);
    // edge_insert(dict, EDGES_Y1, EDGES_X4, 2, 0, -1, -1);
    // edge_insert(dict, EDGES_Y1, EDGES_X5, 2, 5, -1, -1);
    // edge_insert(dict, EDGES_Y1, EDGES_X6, 2, 4, 4, 2);
    // edge_insert(dict, EDGES_Y1, EDGES_X7, 0, 5, 4, 3);
    // edge_insert(dict, EDGES_Y1, EDGES_X8, 0, 4, 0, 2);
    // edge_insert(dict, EDGES_Y1, EDGES_X9, 0, 3, 18, slice_number);
    // edge_insert(dict, EDGES_Y1, EDGES_X10, 0, 0, 2, 3);
    // edge_insert(dict, EDGES_Y1, EDGES_X11, 1, 5, 2, 2);


    // vertex_insert(dict, VERTICES_Y1, VERTICES_X0, 4, 0, -1, -1, -1, -1);
    // vertex_insert(dict, VERTICES_Y1, VERTICES_X1, 4, 5, 5, 1, -1, -1);
    // vertex_insert(dict, VERTICES_Y1, VERTICES_X2, 5, 0, -1, -1, -1, -1);
    // vertex_insert(dict, VERTICES_Y1, VERTICES_X3, 5, 5, -1, -1, -1, -1);
    // vertex_insert(dict, VERTICES_Y1, VERTICES_X4, 5, 4, 7, 1, -1, -1);
    // vertex_insert(dict, VERTICES_Y1, VERTICES_X5, 5, 3, 3, 5, 7, 2);
    // vertex_insert(dict, VERTICES_Y1, VERTICES_X6, 3, 4, 6, 1, 7, 3);
    // vertex_insert(dict, VERTICES_Y1, VERTICES_X7, 3, 3, 6, 2, 18, slice_number);
    // vertex_insert(dict, VERTICES_Y1, VERTICES_X8, 3, 0, 4, 4, 5, 2);
}
