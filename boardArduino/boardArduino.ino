#include "boardArduino.h"

// players[0] == blue
// players[1] == red
// players[2] == green
// players[3] == orange
player_t players[4];

ControllerColors players_move = blueController;
Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIN, NEO_GRB + NEO_KHZ800);

char endTurnButtonState;
char diceRollReadButtonState;

// Digital read for the bool edgeOrVertexPressedState. Please use variable name below!!
bool edgeOrVertexPressedState;


lookup_t *dict = (lookup_t *) malloc(sizeof(lookup_t));
board_t *board_state = NULL;

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
    pinMode(EDGES_Y5, INPUT_PULLUP);
}


void initRing() {

    Serial.write(PLAYER0);
    for (int i = 0; i < 20; i++) Serial.write(0);
}


void ringMessage() {
    Serial1.write('s');

    for(int i = 0; i < 4; i++) {
        Serial1.write(PLAYER0 + i);
        Serial1.write(players[i].woodCount);
        Serial1.write(players[i].sheepCount);
        Serial1.write(players[i].clayCount);
        Serial1.write(players[i].wheatCount);
        Serial1.write(players[i].rockCount);
        Serial1.write(0x0);
        Serial.write(0x0);
    }
    Serial1.write('e');

    for (int i = 0; i < 4; i++) {
        players[i].woodCount = 0;
        players[i].sheepCount = 0;
        players[i].clayCount = 0;
        players[i].wheatCount = 0;
        players[i].rockCount = 0;
    }
}

board_t *initBoard() {
    resource_t resources[19] = {desert, wood, wood, wood, wood, clay, clay, clay, sheep, sheep, sheep, sheep, wheat, wheat, wheat, wheat, rock, rock, rock};
    int nums[18] = {2, 3, 3, 4, 4, 5, 5, 6, 6, 8, 8, 9, 9, 10, 10, 11, 11, 12};

    shuffle((int *) resources, 19);
    shuffle(nums, 18);


    board_t *game_board = (board_t *) malloc(sizeof(board_t));

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

    return game_board;
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
    Serial2.begin(9600);
    Serial1.begin(9600);
    Serial.begin(9600);
    pixels.begin();

    initDict(dict);
    initPins();
    //initRing();
    board_state = initBoard();
    initPlayers();

    delay(3000);

    //ringMessage();

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
        // CHANGED FOR TESTING
        for(int j = REEDSW_Y0; j < REEDSW_Y1 + 1; j++) {
            // We are assigning the tile number based on its location in the reed switch matrix
            if (digitalRead(j) == LOW) {
                Serial.print(i);
                Serial.print(", ");
                Serial.print(j);
                size_t tile = reedsw_lookup(dict, i, j);
                return tile == -1 ? 18 : tile;
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
            if (digitalRead(j) == LOW) {
                int edgeRow = (i - EDGES_X0) >> 1;
                int edgeCol = (j - EDGES_Y0) >> 1;

                return edge_lookup(dict, j, i);
            }
        }
        digitalWrite(i, HIGH);
    }

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
            if (digitalRead(j) == LOW) {
                int edgeRow = (i - EDGES_X0) >> 1;
                int edgeCol = (j - EDGES_Y0) >> 1;
                int *retVal = (int *) malloc(2 * sizeof(int));
                retVal[0] = edgeRow;
                retVal[1] = edgeCol;
                return retVal;
            }
        }
        digitalWrite(i, HIGH);
    }

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
            if (digitalRead(j) == LOW) {
                delay(40);
                if (digitalRead(j) != LOW) continue;
                int vertexRow = i - VERTICES_X0;
                int vertexCol = j - VERTICES_Y0;
                
                return vertex_lookup(dict, j, i);
            }
        }
        digitalWrite(i, HIGH);
    }

    return NULL;
}

int *testVertices() {
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
            if (digitalRead(j) == LOW) {
                int vertexRow = i - VERTICES_X0;
                int vertexCol = j - VERTICES_Y0;

                int *retVal = (int *) malloc(2 * sizeof(int));
                retVal[0] = vertexRow;
                retVal[1] = vertexCol;
                return retVal;
            }
        }
        digitalWrite(i, HIGH);
    }

    return NULL;
}

void checkButtonStates() {
  char currRead;
  int iter = 0;

  if (Serial.available() <= 0) {
    return
  } else {
    currRead = Serial.read();
  }
  
  // Message parsing and sending
  while ((currRead != 'e')) {
    buf[iter] = currRead;
    iter++;
    while (Serial.available() <= 0);
    currRead = Serial.read();
  }
  buf[iter] = 'e';
  iter++;

    if (iter != 1) {
        buf[iter] = '\0';
    }

    endTurnButtonState = 0x0;
    diceRollReadButtonState = 0x0;


    for (int i = 0; i < iter; i+=8) {
        switch (buf[i]) {
            case (blueController):
                if (players_move == blueController) {
                    endTurnButtonState = buf[i+6];
                    diceRollReadButtonState = buf[i+7];
                }
                break;
            case (redController):
                if (players_move == redController) {
                    endTurnButtonState = buf[i+6];
                    diceRollReadButtonState = buf[i+7];
                }
                break;
            case (orangeController):
                if (players_move == orangeController) {
                    endTurnButtonState = buf[i+6];
                    diceRollReadButtonState = buf[i+7];
                }
                break;
            case (greenController):
                if (players_move == orangeController) {
                    endTurnButtonState = buf[i+6];
                    diceRollReadButtonState = buf[i+7];
                }
                break
            default case:
                break;
        }
    }
}

playerOnVertex_t playerOnVertex(building_t type) {
    playerOnVertex_t playerInfo;
    // checking vertices on the tiles
    for (int i = 0; i < 6; i++) {
        switch(tile.vertices[i].type) {
            case (none): 
                playerInfo.itemCount = -1;
                playerInfo.playerIndex = 0; 
                break;
            case (blue_settlement): 
                playerInfo.itemCount = 1;
                playerInfo.playerIndex = 0; 
                break;   
            case (blue_city):
                playerInfo.itemCount = 2;
                playerInfo.playerIndex = 0; 
                break;
            case (green_settlement):
                playerInfo.itemCount = 1;
                playerInfo.playerIndex = 1; 
                break;
            case (green_city):
                playerInfo.itemCount = 2;
                playerInfo.playerIndex = 1; 
                break;
            case (red_settlement):
                playerInfo.itemCount = 1;
                playerInfo.playerIndex = 2; 
                break;
            case (red_city):
                playerInfo.itemCount = 2;
                playerInfo.playerIndex = 2; 
                break;
            case (orange_settlement):
                playerInfo.itemCount = 1;
                playerInfo.playerIndex = 3; 
                break;
            case (orange_city):
                playerInfo.itemCount = 2;
                playerInfo.playerIndex = 3; 
                break;
            default case:
                playerInfo.itemCount = -1;
                playerInfo.playerIndex = 0; 
                break; 
        }
    }
    return playerInfo;
}

void passOutCards(int diceRollTotal) {
    for (int i = 0; i < 19; i++) {
        if (game_board->tiles[i].num == diceRollTotal) {
            switch (game_board->tiles[i].type) {
                case (desert):
                    break;
                case (rock):
                    for (int j = 0; j < 6; j++) {
                        playerOnVertex_t playerInfo = playerOnVertex(game_board->tiles[i].vertices[j].type);
                        if (playerInfo.playerIndex != -1) {
                            players[playerInfo.playerIndex].rockCount += playerInfo.itemCount;
                        }
                    }
                    break;
                case (clay):
                    for (int j = 0; j < 6; j++) {
                        playerOnVertex_t playerInfo = playerOnVertex(game_board->tiles[i].vertices[j].type);
                        if (playerInfo.playerIndex != -1) {
                            players[playerInfo.playerIndex].clayCount += playerInfo.itemCount;
                        }
                    }
                    break;
                case (wood):
                    for (int j = 0; j < 6; j++) {
                        playerOnVertex_t playerInfo = playerOnVertex(game_board->tiles[i].vertices[j].type);
                        if (playerInfo.playerIndex != -1) {
                            players[playerInfo.playerIndex].woodCount += playerInfo.itemCount;
                        }
                    }
                    break;
                case (wheat):
                    for (int j = 0; j < 6; j++) {
                        playerOnVertex_t playerInfo = playerOnVertex(game_board->tiles[i].vertices[j].type);
                        players[playerInfo.playerIndex].wheatCount += playerInfo.itemCount;
                    }
                    break;
                case (sheep):
                    for (int j = 0; j < 6; j++) {
                        playerOnVertex_t playerInfo = playerOnVertex(game_board->tiles[i].vertices[j].type);
                        if (playerInfo.playerIndex != -1) {
                            players[playerInfo.playerIndex].sheepCount += playerInfo.itemCount;
                        }
                    }
                    break;
                default case:
                    break;
            }
        }
    }
}

bool edgeOrVertexPressed = false;
bool prevEdgeOrVertexPressed = false;
bool readDice = false;
bool prevDiceButtonState = false;
    
void loop() {
    // Recieve data from dice
    if (diceRollReadButtonState != prevDiceRollButtonState) {
        if (diceRollReadButtonState == true) {
            readDice = true;
        }
    }
    prevDiceRollButtonState = diceRollButtonState;

    if (Serial2.available() > 0 & readDice) {
        int diceRollTotal = Serial2.read();
        
        // Set the player resource variables in this function
        // NEED TO CALL pollReedSwitch to check where robber is in here
        passOutCards(diceRollTotal);

        // Send the message with the cards
        ringMessage();
        readDice = false;
    }

    // Set vertexState when an vertex is pressed
    if (edgeOrVertexPressedState != prevEdgeOrVertexState) {
        if (edgeOrVertexPressedState == true) {
            edgeOrVertexPressed = true;
        }
    }
    prevEdgeOrVertexPressed = edgeOrVertexPressedState;
    
    if (endTurnButtonState) {
        endTurnButtonState = 0x0;
        players_move = (players_move == orangeController) ? blueController : players_move + 1;
    }

    int *activeEdge = pollEdges();
    int *activeVertex = pollVertices();

    // Account for edges on one tile ********
    // update Edges based on active pressed
    if (activeEdge != NULL & edgeOrVertexPressed) {
        // Tile1, Edge1, Tile2, Edge2    
        if (activeVertex[0] != -1) {
            building_t roadType1 = board_state->tiles[activeEdge[0]].edges[activeEdge[1]].type;
            board_state->tiles[activeEdge[0]].edges[activeEdge[1]].type = updateRoadColor(roadType1);
        }
        
        if (activeVertex[2] != -1) {
            building_t roadType2 = board_state->tiles[activeEdge[2]].edges[activeEdge[3]].type;
            board_state->tiles[activeVertex[2]].edges[activeEdge[3]].type = updateRoadColor(roadType2);
        }
    }

    // update Vertices based on active pressed
    else if (activeVertex != NULL & edgeOrVertexPressed) {
        // [Tile1, Vert1, Tile2, Vert2, Tile3, Vert3]
        // Need to figure out which tiles have active vertexes and update all of them
        if (activeVertex[0] != -1) {
            building_t buildingType1 = board_state->tiles[activeVertex[0]].vertices[activeVertex[1]].type;
            board_state->tiles[activeVertex[0]].vertices[activeVertex[1]].type = updateBuildingType(buildingType1);
        }
        
        if (activeVertex[2] != -1) {
            building_t buildingType2 = board_state->tiles[activeVertex[2]].vertices[activeVertex[3]].type;
            board_state->tiles[activeVertex[2]].vertices[activeVertex[3]].type = updateBuildingType(buildingType2);
        }
        if (activeVertex[4] != -1) {
            building_t buildingType3 = board_state->tiles[activeVertex[4]].vertices[activeVertex[5]].type;
            board_state->tiles[activeVertex[4]].vertices[activeVertex[5]].type = updateBuildingType(buildingType3);
        }
    }
    edgeOrVertexPressed = false;
    
    updatePixelColors();
    
    // TESTING CODE BELOW HERE, DONT MODIFY
    continue;    
    while(1) {
      pixels.clear();
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setBrightness(255);
        pixels.setPixelColor(i, pixels.Color(0, 150, 0));
        delay(250);
        pixels.show();
      }
    }

    int val;
    int *res;
    while(1) {
        if((val = pollReedSwitch()) != 18) {
            Serial.print("Robber value is ");
            Serial.println(val);
        }
        if((res = testEdges()) != NULL) {
            Serial.print("EDGEX, EDGEY: ");
            Serial.print(res[0]);
            Serial.print(", ");
            Serial.println(res[1]);

            free(res);
            res = NULL;
        }
        if((res = testVertices()) != NULL) {
            Serial.print("VERTX, VERTY: ");
            Serial.print(res[0]);
            Serial.print(", ");
            Serial.println(res[1]);

            free(res);
            res = NULL;
        }
    }
    
    ringMessage();

    int aval = Serial1.available();
    for (int i = 0; i < aval; i++) {
        Serial.println((Serial1.read()));
    }
}

building_t updateBuildingType(building_t buildingType) {
    if (buildingType == none) {
        switch(players_move) {
            case (blueController): 
                return blue_settlement;
            case (redController):
                return red_settlement;
            case (greenController):
                return green_settlement;
            case (orangeController):
                return orange_settlement;
            default case:
                return none;
        }
    }
    else if (buildingType == blue_settlement | buildingType == green_settlement 
            buildingType == red_settlement | buildingType == orange_settlement) {
        switch(players_move) {
            case (blueController): 
                return blue_city;
            case (redController):
                return red_city;
            case (greenController):
                return green_city;
            case (orangeController):
                return orange_city;
            default case:
                return none;
        }
    }
    else {
        return none;
    }
}

building_t updateRoadColor(building_t buildingType) {
    if (buildingType == none) {
        switch(players_move) {
            case (blueController): 
                return blue_settlement;
            case (redController):
                return red_settlement;
            case (greenController):
                return green_settlement;
            case (orangeController):
                return orange_settlement;
            default case:
                return none;
        }
    }
    else {
        return none;
    }
}

void updatePixelColors() {
    for (int i = 0; i < 19; i++) {
        // for edges
        for (int j = 0; j < 6; j ++) {
            edge_t edge = game_board->tiles[i].edges[j];
            switch (edge.type) {
                case (none):
                    pixels.setPixelColor(edge.ledID1, pixels.Color(0, 0, 0));
                    pixels.setPixelColor(edge.ledID2, pixels.Color(0, 0, 0));
                    break;
                case (blue_settlement | blue_city):
                    pixels.setPixelColor(edge.ledID1, pixels.Color(0, 0, 150));
                    pixels.setPixelColor(edge.ledID2, pixels.Color(0, 0, 150));
                    break;
                case (green_settlement | green_city):
                    pixels.setPixelColor(edge.ledID1, pixels.Color(0, 150, 0));
                    pixels.setPixelColor(edge.ledID2, pixels.Color(0, 150, 0));
                    break;
                case (red_settlement | red_city):
                    pixels.setPixelColor(edge.ledID1, pixels.Color(150, 0, 0));
                    pixels.setPixelColor(edge.ledID2, pixels.Color(150, 0, 0));
                    break;
                case (orange_settlement | orange_city):
                    pixels.setPixelColor(edge.ledID1, pixels.Color(150, 75, 0));
                    pixels.setPixelColor(edge.ledID2, pixels.Color(150, 75, 0));
                    break;
                default case:
                    break;
            }
        }
        for (int k = 0; k < 6; k++) {
            vertex_t vertex = game_board->tiles[i].vertices[k];
            switch (game_board->tiles[i].vertices[k].type) {
                switch (vertex.type) {
                    case (none):
                        pixels.setPixelColor(vertex.ledID, pixels.Color(0, 0, 0));
                        break;
                    case (blue_settlement | blue_city):
                        pixels.setPixelColor(vertex.ledID, pixels.Color(0, 0, 150));
                        break;
                    case (green_settlement | green_city):
                        pixels.setPixelColor(vertex.ledID, pixels.Color(0, 150, 0));
                        break;
                    case (red_settlement | red_city):
                        pixels.setPixelColor(vertex.ledID, pixels.Color(150, 0, 0));
                        break;
                    case (orange_settlement | orange_city):
                        pixels.setPixelColor(vertex.ledID, pixels.Color(150, 75, 0));
                        break;
                    default case:
                        break;
                }   
            }
        }
    }
}

