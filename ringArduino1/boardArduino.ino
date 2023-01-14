#include <SoftwareSerial.h>
#include <Arduino.h>


#define DESERT 0
#define ROCK 1
#define CLAY 2
#define WHEAT 3
#define WOOD 4
#define SHEEP 5

// Serial
#define rxPin 10
#define txPin 11

// Set up a new SoftwareSerial object
SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);


void place(int tile_type, int *tile_indices, tile_count) {
    for (int i = 0; i < tile_count; i++) {
        switch(tile_type) {
            case DESERT: break;
            case ROCK: break;
            case CLAY: break;
            case WHEAT: break;
            case WOOD: break;
            case SHEEP: break;
            default:
        }
    }

}

void setup() {
    // Setup for Software Serial
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    mySerial.begin(9600);
}

void main() {
    char[19] tiles;
    int desert_ind = -1;
    int sheep_ind = 0;
    int rock_ind = 0;
    int clay_ind = 0;
    int wood_ind = 0;
    int wheat_ind = 0;
    int[3] rock_indices;
    int[3] clay_indices;
    int[4] wheat_indices;
    int[4] wood_indices;
    int[4] sheep_indices;

    // Message parsing and sending
    if (mySerial.available() > 0) {
        if (mySerial.read() == 's') {

            // Stream starts with an s and ends with an e
            for(int i = 0; i < 19; i++) {
                tiles[i] = mySerial.read();
            }

            // Read the end of message e
            mySerial.read();
        }

        // Start tile placing sequence
        // Place desert
        for (int i = 0; i < 19; i++) {
            switch(tiles[i]) {
                case DESERT: desert_ind = i; break;
                case ROCK: rock_indices[rock_ind] = i; rock_ind++; break;
                case CLAY: clay_indices[clay_ind] = i; clay_ind++; break;
                case WHEAT: wheat_indices[wheat_ind] = i; wheat_ind++; break;
                case WOOD: wood_indices[wood_ind] = i; wood_ind++; break;
                case SHEEP: sheep_indices[sheep_ind] = i; sheep_ind++; break;
                default:
            }
        }

        place(DESERT, &desert_ind, 1);
        place(ROCK, rock_indices, rock_ind);
        place(CLAY, clay_indices, clay_ind);
        place(WHEAT, wheat_indices, wheat_ind);
        place(WOOD, wood_indices, wood_ind);
        place(SHEEP, sheep_indices, sheep_ind);
    }
}