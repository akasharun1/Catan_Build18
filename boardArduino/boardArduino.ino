#include "boardArduino.h"

player_t players[4];

void shuffle(int *array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
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
    pinMode(REEDSW_Y5, INPUT_PULLUP);

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
    for (int i = 0; i < 20; i++) Serial.write(0);
}


void ringMessage() {

    players[0].woodCount = 5;
    players[0].sheepCount = 4;
    players[0].clayCount = 3;
    players[0].wheatCount = 2;
    players[0].rockCount = 1;

    
    players[1].woodCount = 5;
    players[1].sheepCount = 4;
    players[1].clayCount = 3;
    players[1].wheatCount = 2;
    players[1].rockCount = 1;

    
    players[2].woodCount = 5;
    players[2].sheepCount = 4;
    players[2].clayCount = 3;
    players[2].wheatCount = 2;
    players[2].rockCount = 1;

    players[3].woodCount = 5;
    players[3].sheepCount = 4;
    players[3].clayCount = 3;
    players[3].wheatCount = 2;
    players[3].rockCount = 1;  

    
    
    Serial1.write('s');

    for(int i = 0; i < 4; i++) {
        Serial1.write(PLAYER0 + i);
        Serial1.write(players[i].woodCount);
        Serial1.write(players[i].sheepCount);
        Serial1.write(players[i].clayCount);
        Serial1.write(players[i].wheatCount);
        Serial1.write(players[i].rockCount);
    }
    Serial1.write('e');

}

void initBoard() {
    resource_t resources[19] = {desert, wood, wood, wood, wood, clay, clay, clay, sheep, sheep, sheep, sheep, wheat, wheat, wheat, wheat, rock, rock, rock};
    int nums[18] = {2, 3, 3, 4, 4, 5, 5, 6, 6, 8, 8, 9, 9, 10, 10, 11, 11, 12};

    shuffle((int *) resources, 19);
    shuffle(nums, 18);


    board_t *game_board = (board_t *) malloc(sizeof(board_t));
    //game_board->tiles = (tile_t *) malloc(19 * sizeof(tile_t));

    int numInd;
    for (int i = 0; i < 19; i++){
        game_board->tiles[i].type = resources[i];
        if (resources[i] == desert) {
            game_board->tiles[i].num =  -1;
        } else {
            game_board->tiles[i].num = nums[numInd];
            numInd++;
        }
    }

    //return game_board;
    return;
}

void initPlayers() {

    for (int i = 0; i < 4; i++) {
        players[i].woodCount = 0;
        players[i].sheepCount = 0;
        players[i].clayCount = 0;
        players[i].wheatCount = 0;
        players[i].rockCount = 0;
    }
}

void setup() {
    Serial1.begin(9600);
    Serial.begin(9600);

    initPins();
    initRing();
    initBoard();
    initPlayers();

    delay(3000);

    ringMessage();

//    Serial.println("Done Ring msg");
//
//    delay(1000);
//  
//    int aval = Serial1.available();
//
//    for (int i = 0; i < aval; i++) {
//      Serial.println((Serial1.read()));
//    }


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


int *pollEdges() {
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

                //return button_ind_to_tileEdge_ind[make_pair(edgeRow, edgeCol)].second;
                return NULL;
            }
        }
        digitalWrite(i, HIGH);
    }

    //return make_pair(-1, -1);
    return NULL;
}

int *testEdges() {
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
                //return make_pair(edgeRow, edgeCol);
                return NULL;
            }
        }
        digitalWrite(i, HIGH);
    }

    //return make_pair(-1, -1);
    return NULL;
}



int *pollVertices() {
    digitalWrite(VERTICES_X0, HIGH);
    digitalWrite(VERTICES_X1, HIGH);
    digitalWrite(VERTICES_X2, HIGH);
    digitalWrite(VERTICES_X3, HIGH);
    digitalWrite(VERTICES_X4, HIGH);
    digitalWrite(VERTICES_X5, HIGH);
    digitalWrite(VERTICES_X6, HIGH);
    digitalWrite(VERTICES_X7, HIGH);
    digitalWrite(VERTICES_X8, HIGH);


    for (int i = VERTICES_X0; i < VERTICES_X8 + 1; i++) {
        digitalWrite(i, LOW);
        for(int j = VERTICES_Y0; j < VERTICES_Y5 + 1; j++) {
            // We are assigning the tile number based on its location in the reed switch matrix
            if (digitalRead(j) == 0) {
                int vertexRow = i - VERTICES_X0;
                int vertexCol = j - VERTICES_Y0;
                //return button_ind_to_tileVertex_ind[make_pair(vertexRow, vertexCol)].second;
                return NULL;
            }
        }
        digitalWrite(i, HIGH);
    }

    //return make_pair(-1, -1);
    return NULL;
}

void testVertices() {
    digitalWrite(VERTICES_X0, HIGH);
    digitalWrite(VERTICES_X1, HIGH);
    digitalWrite(VERTICES_X2, HIGH);
    digitalWrite(VERTICES_X3, HIGH);
    digitalWrite(VERTICES_X4, HIGH);
    digitalWrite(VERTICES_X5, HIGH);
    digitalWrite(VERTICES_X6, HIGH);
    digitalWrite(VERTICES_X7, HIGH);
    digitalWrite(VERTICES_X8, HIGH);
 

    for (int i = VERTICES_X0; i < VERTICES_X8 + 1; i++) {
        digitalWrite(i, LOW);
        for(int j = VERTICES_Y0; j < VERTICES_Y5 + 1; j++) {
            // We are assigning the tile number based on its location in the reed switch matrix
            if (digitalRead(j) == 0) {
                int vertexRow = i - VERTICES_X0;
                int vertexCol = j - VERTICES_Y0;

                Serial.println("vertex row is:");
                Serial.println(vertexRow);
                Serial.println("vertex col is:");
                Serial.println(vertexCol);
                return;
            }
        }
        digitalWrite(i, HIGH);
    }

    Serial.println(-1);
}


void loop() {
    
    Serial.println("start ring msg");
    ringMessage();
//
    Serial.println("Done Ring msg");
//
    delay(1000);
//  
    int aval = Serial1.available();
//
    for (int i = 0; i < aval; i++) {
      Serial.println((Serial1.read()));
    }



    //Serial.println(Serial1.read());
    //while (Serial1.available() > 0) Serial.println(Serial1.read());
    
 







    

    

}
