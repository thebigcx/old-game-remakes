#pragma once

#include <SDL2/SDL.h>

#include "bool.h"

// Tile render size
#define BOARD_SIZE 16
#define MINE_COUNT 40
#define TILE_R_SIZE (500 / BOARD_SIZE)

struct Tile
{
    bool mine;
    bool hidden;
    bool flagged;
    int num_adj_mines;
    bool mine_clicked;
    bool incorrect_flag;
    bool recurse; // Prevent recursion problems when tiles reveal each other infinitely
};

struct Vector2
{
    int x, y;
};

extern SDL_Texture* tile_texture_atlas;

struct GlobalState;

void generate_board(struct GlobalState* state);
void init_tile_texture_atlas(SDL_Renderer* renderer);
void render_tiles(SDL_Renderer* renderer, struct GlobalState* state);
struct Vector2 get_tile_uv(struct Tile* tile);
void on_click(struct GlobalState* state, int x, int y, bool is_left_click);
void reveal_mines(struct GlobalState* state);
int get_adj_mines(struct GlobalState* state, int x, int y);
void reveal_adjacents(struct GlobalState* state, int x, int y);
void reveal(struct GlobalState* state, int x, int y);
void auto_reveal_adjacents(struct GlobalState* state, int x, int y);
void check_completion(struct GlobalState* state);
