import enum
import random

class tiles(enum.Enum):
    DESERT = 0
    ROCK = 1
    CLAY = 2
    WHEAT = 3
    WOOD = 4
    SHEEP = 5

Tile_counts = {}
Tile_counts[tiles.DESERT] = 1
Tile_counts[tiles.ROCK] = 3
Tile_counts[tiles.CLAY] = 3
Tile_counts[tiles.WHEAT] = 4
Tile_counts[tiles.WOOD] = 4
Tile_counts[tiles.SHEEP] = 4

tile_list = []
for key, value in Tile_counts:
    for i in range(value):
        tile_list.append(key)

# Randomly ordered list of all tiles
random.shuffle(tile_list)

# Enumerate tiles with position
enumerated_tiles = enumerate(tile_list)



# Send tile list to the board

# Initialize the board
