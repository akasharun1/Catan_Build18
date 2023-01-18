#include <Arduino.h>
using namespace std;
#include <map>
#include <vector>
#include <algorithm>

#define PLAYER0 0x60

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
#define EGDES_Y3 49
#define EDGES_Y4 51
#define EDGES_Y5 53
#define EDGES_Y_NUM 6


#define DiceTX 16
#define DiceRX 17

#define ArduinoRingTX 18
#define ArduinoRingRX 19

#define piDisplayTX 14
#define piDisplayRX 15


map<pair<int, int>, pair<int, int>> button_ind_to_tileVertex_ind;
map<pair<int, int>, pair<int, int>> button_ind_to_tileEdge_ind;
map<pair<int, int>, pair<int, int>>::iterator iter;

// Assign hardcoded values of buttons to tile and vertices here. This is a dummy example
button_ind_to_tileVertex_ind[make_pair(0, 2)] = make_pair(2, 3);

typedef enum {
    desert = 0,
    rock,
    clay,
    wheat,
    wood,
    sheep
} resource_t;

typedef enum {
    num_desert = 1,
    num_rock = 3,
    num_clay = 3,
    num_wheat = 4,
    num_wood = 4,
    num_sheep = 4
} resource_count_t;

typedef enum {
    none = 0,
    blue_settlement,
    blue_city,
    green_settlement,
    green_city,
    red_settlement,
    red_city,
    orange_settlement,
    orange_city
} building_t;

typedef struct vertex {
    building_t type;
    int ledID;
} vertex_t;

typedef struct edge {
    building_t type;
    int ledID1;
    int ledID2;
} edge_t;

typedef struct tile_comp {
    vertex_t[6] vertices;
    edge_t[6] edges;
    resource_t type;
    int num;
} tile_t;

typedef struct catan_board {
    tile_t[19] tiles; // Tile IDs are assigned by reed switch value
} board_t;

int Rand(int n)
{
    return rand() % n ;
}


void initPins() {
    pinMode(REEDSW_X0, OUTPUT);
    pinMode(REEDSW_X1, OUTPUT);
    pinMode(REEDSW_X2, OUTPUT);

    pinMode(REEDSW_Y0, INPUT_PULLUP);
    pinMode(REEDSW_Y1, INPUT_PULLUP);
    pinMode(REEDSW_Y2, INPUT_PULLUP);
    pinMode(REEDSW_Y3, INPUT_PULLUP);
    pinMode(REEDSW_Y4, INPUT_PULLUP);
    pinMode(REEDSW_X5, INPUT_PULLUP);
    pinMode(REEDSW_X6, INPUT_PULLUP);

    pinMode(VERTICES_X0, OUTPUT);
    pinMode(VERTICES_X1, OUTPUT);
    pinMode(VERTICES_X2, OUTPUT);
    pinMode(VERTICES_X3, OUTPUT);
    pinMode(VERTICES_X4, OUTPUT);
    pinMode(VERTICES_X5, OUTPUT);
    pinMode(VERTICES_X6, OUTPUT);
    pinMode(VERTICES_X7, OUTPUT);
    pinMode(VERTICES_X8, OUTPUT);

    pinMode(VERTICES_Y0, INPUT_PULLUP);
    pinMode(VERTICES_Y1, INPUT_PULLUP);
    pinMode(VERTICES_Y2, INPUT_PULLUP);
    pinMode(VERTICES_Y3, INPUT_PULLUP);
    pinMode(VERTICES_Y4, INPUT_PULLUP);
    pinMode(VERTICES_Y5, INPUT_PULLUP);

    pinMode(EDGES_X0, OUTPUT);
    pinMode(EDGES_X1, OUTPUT);
    pinMode(EDGES_X2, OUTPUT);
    pinMode(EDGES_X3, OUTPUT);
    pinMode(EDGES_X4, OUTPUT);
    pinMode(EDGES_X5, OUTPUT);
    pinMode(EDGES_X6, OUTPUT);
    pinMode(EDGES_X7, OUTPUT);
    pinMode(EDGES_X8, OUTPUT);
    pinMode(EDGES_X9, OUTPUT);
    pinMode(EDGES_X10, OUTPUT);
    pinMode(EDGES_X11, OUTPUT);

    pinMode(EDGES_Y0, INPUT_PULLUP);
    pinMode(EDGES_Y1, INPUT_PULLUP);
    pinMode(EDGES_Y2, INPUT_PULLUP);
    pinMode(EDGES_Y3, INPUT_PULLUP);
    pinMode(EDGES_Y4, INPUT_PULLUP);
    pinMode(EDGES_X5, INPUT_PULLUP);
}



void initRing() {
    Serial.write(PLAYER0);

    Serial.write('s');
    for(int player = 0; player < 4; player++) {
        Serial.write(PLAYER0 + player);
        Serial.write(0);
        Serial.write(0);
        Serial.write(0);
        Serial.write(0);
        Serial.write(0);
    }
    Serial.write('e');
}

board_t initBoard() {
    vector<resource_t> resources = {desert, wood, wood, wood, wood, clay, clay, clay, sheep, sheep, sheep, sheep, wheat, wheat, wheat, wheat, rock, rock, rock};
    vector<int> nums = {2, 3, 3, 4, 4, 5, 5, 6, 6, 8, 8, 9, 9, 10, 10, 11, 11, 12}

    random_shuffle(resources.begin(), resources.end());
    random_shuffle(nums.begin(), nums.end());

    board_t game_board = new board_t;
    game_board.tiles = new tile_t[19];

    int numInd;
    for (int i = 0; i < 19; i++){
        game_board.tiles[i].type = resources[i];
        if (resources[i] == desert) {
            game_board.tiles[i].num =  -1;
        } else {
            game_board.tiles[i].num = nums[numInd];
            numInd++;
        }
    }
    return game_board;
}

void setup() {
    Serial.begin(9600);

    initPins();
    initRing();
    initBoard();

}


int pollReedSwitch() {
    digitalWrite(REEDSW_X0, HIGH);
    digitalWrite(REEDSW_X1, HIGH);
    digitalWrite(REEDSW_X2, HIGH);

    for (int i = REEDSW_X0; i < REEDSW_X2 + 1; i++) {
        digitalWrite(i, LOW);
        for(int j = REEDSW_Y0; i < REEDSW_Y5 + 1; j++) {
            // We are assigning the tile number based on its location in the reed switch matrix
            if (digitalRead(j) == LOW) {
                int reedRow = i + 1 - REEDSW_X0;
                int reedCol = j + 1 - REEDSW_Y0;
                return (reedRow) * (reedCol);
            }
        }
        digitalWrite(i, HIGH);
    }
    // The robber was not found so it's in the middle
    return 18;
}


pair<int, int> pollEdges() {
    digitalWrite(EDGES_X0, HIGH);
    digitalWrite(EDGES_X1, HIGH);
    digitalWrite(EDGES_X2, HIGH);
    digitalWrite(EDGES_X3, HIGH);
    digitalWrite(EDGES_X4, HIGH);
    digitalWrite(EDGES_X5, HIGH);
    digitalWrite(EDGES_X6, HIGH);
    digitalWrite(EDGES_X7, HIGH);
    digitalWrite(EDGES_X8, HIGH);
    digitalWrite(EDGES_X9, HIGH);
    digitalWrite(EDGES_X10, HIGH);
    digitalWrite(EDGES_X11, HIGH);

    for (int i = EDGES_X0; i < EDGES_X11 + 1; i += 2) {
        digitalWrite(i, LOW);
        for(int j = EDGES_Y0; j < EDGES_Y5 + 1; j += 2) {
            // We are assigning the tile number based on its location in the reed switch matrix
            if (digitalRead(j) == 0) {
                int edgeRow = (i - EDGES_X0) >> 1;
                int edgeCol = (j - EDGES_Y0) >> 1;
                return button_ind_to_tileEdge_ind[make_pair(edgeRow, edgeCol)].second;
            }
        }
        digitalWrite(i, HIGH);
    }

    return make_pair(-1, -1);
}

pair<int, int> testEdges() {
    digitalWrite(EDGES_X0, HIGH);
    digitalWrite(EDGES_X1, HIGH);
    digitalWrite(EDGES_X2, HIGH);
    digitalWrite(EDGES_X3, HIGH);
    digitalWrite(EDGES_X4, HIGH);
    digitalWrite(EDGES_X5, HIGH);
    digitalWrite(EDGES_X6, HIGH);
    digitalWrite(EDGES_X7, HIGH);
    digitalWrite(EDGES_X8, HIGH);
    digitalWrite(EDGES_X9, HIGH);
    digitalWrite(EDGES_X10, HIGH);
    digitalWrite(EDGES_X11, HIGH);

    for (int i = EDGES_X0; i < EDGES_X11 + 1; i += 2) {
        digitalWrite(i, LOW);
        for(int j = EDGES_Y0; j < EDGES_Y5 + 1; j += 2) {
            // We are assigning the tile number based on its location in the reed switch matrix
            if (digitalRead(j) == 0) {
                int edgeRow = (i - EDGES_X0) >> 1;
                int edgeCol = (j - EDGES_Y0) >> 1;
                return make_pair(edgeRow, edgeCol);
            }
        }
        digitalWrite(i, HIGH);
    }

    return make_pair(-1, -1);
}



pair<int, int> pollVertices() {
    digitalWrite(VERTICES_X0, HIGH);
    digitalWrite(VERTICES_X1, HIGH);
    digitalWrite(VERTICES_X2, HIGH);
    digitalWrite(VERTICES_X3, HIGH);
    digitalWrite(VERTICES_X4, HIGH);
    digitalWrite(VERTICES_X5, HIGH);
    digitalWrite(VERTICES_X6, HIGH);
    digitalWrite(VERTICES_X7, HIGH);
    digitalWrite(VERTICES_X8, HIGH);
    digitalWrite(VERTICES_X9, HIGH);
    digitalWrite(VERTICES_X10, HIGH);
    digitalWrite(VERTICES_X11, HIGH);

    for (int i = VERTICES_X0; i < VERTICES_X8 + 1; i++) {
        digitalWrite(i, LOW);
        for(int j = VERTICES_Y0; j < VERTICES_Y5 + 1; j++) {
            // We are assigning the tile number based on its location in the reed switch matrix
            if (digitalRead(j) == 0) {
                int vertexRow = i - VERTICES_X0;
                int vertexCol = j - VERTICES_Y0;
                return button_ind_to_tileVertex_ind[make_pair(vertexRow, vertexCol)].second;
            }
        }
        digitalWrite(i, HIGH);
    }

    return make_pair(-1, -1);
}

pair<int, int> testVertices() {
    digitalWrite(VERTICES_X0, HIGH);
    digitalWrite(VERTICES_X1, HIGH);
    digitalWrite(VERTICES_X2, HIGH);
    digitalWrite(VERTICES_X3, HIGH);
    digitalWrite(VERTICES_X4, HIGH);
    digitalWrite(VERTICES_X5, HIGH);
    digitalWrite(VERTICES_X6, HIGH);
    digitalWrite(VERTICES_X7, HIGH);
    digitalWrite(VERTICES_X8, HIGH);
    digitalWrite(VERTICES_X9, HIGH);
    digitalWrite(VERTICES_X10, HIGH);
    digitalWrite(VERTICES_X11, HIGH);

    for (int i = VERTICES_X0; i < VERTICES_X8 + 1; i++) {
        digitalWrite(i, LOW);
        for(int j = VERTICES_Y0; j < VERTICES_Y5 + 1; j++) {
            // We are assigning the tile number based on its location in the reed switch matrix
            if (digitalRead(j) == 0) {
                int vertexRow = i - VERTICES_X0;
                int vertexCol = j - VERTICES_Y0;
                return make_pair(vertexRow, vertexCol);
            }
        }
        digitalWrite(i, HIGH);
    }

    return make_pair(-1, -1);
}


void loop() {

}
