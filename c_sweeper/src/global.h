#pragma once

#include <time.h>

#include "tile.h"

struct GlobalState
{
    struct Tile tiles[BOARD_SIZE][BOARD_SIZE];
    bool game_over;
    bool did_complete;
    int flags; // Flagged tiles (for determining remaining tiles)
    clock_t time;
};
