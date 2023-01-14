import enum
import random
import pyserial as serial

class tiles(enum.Enum):
    DESERT = 0
    ROCK = 1
    CLAY = 2
    WHEAT = 3
    WOOD = 4
    SHEEP = 5

def init_board(port_name, port, tile_list):
    port_name=raw_input("What serial port did you use during setup? [i.e. /dev/ttyS44] ")
    port=serial.Serial(port=port_name, baudrate=9600)

    # send the tiles
    port.write(chr('s'))
    for tile in tile_list:
        port.write(tile)
    port.write(chr('e'))

    

def main():
    Tile_counts = {}
    Tile_counts[tiles.DESERT] = 1
    Tile_counts[tiles.ROCK] = 3
    Tile_counts[tiles.CLAY] = 3
    Tile_counts[tiles.WHEAT] = 4
    Tile_counts[tiles.WOOD] = 4
    Tile_counts[tiles.SHEEP] = 4

    port = ""
    port_name = ""

    tile_list = []
    for key, value in Tile_counts:
        for i in range(value):
            tile_list.append(key)

    # Randomly ordered list of all tiles
    random.shuffle(tile_list)

    # Enumerate tiles with position
    # enumerated_tiles = enumerate(tile_list)

    # Send tile list to the board and init
    # Initialize the board
    init_board(port_name, port, tile_list)
