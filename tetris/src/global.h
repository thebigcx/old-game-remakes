#pragma once

#include "board.h"

struct GlobalState
{
    struct Board board;
    struct Shape current_shape;
    bool game_over;
    int score;
};
