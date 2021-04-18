#pragma once

#include <SDL2/SDL.h>
#include "bool.h"

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 22
#define CELL_R_SIZE 40

enum PIECE_TYPE
{
    NO_BLOCK, I_BLOCK, J_BLOCK, L_BLOCK, O_BLOCK, S_BLOCK, T_BLOCK, Z_BLOCK
};

struct Board
{
    int cells[BOARD_WIDTH][BOARD_HEIGHT]; // Piece types
};

struct Matrix4
{
    int cells[4][4];
};

struct Shape
{
    int type;
    struct Matrix4 matrix;
    int mat_size;
    int x, y;
};

struct Vector2
{
    float x, y;
};

extern SDL_Texture* cell_textures;
extern struct Shape shapes[8];
extern clock_t timer;
extern float speed;

struct GlobalState;

void clear_line(struct GlobalState* state, int line);
void place_block(struct GlobalState* state);
bool block_colliding(struct GlobalState* state);
void update(struct GlobalState* state);
void key_press(struct GlobalState* state, SDL_KeyboardEvent key);
void init_textures(SDL_Renderer* renderer);
void setup_board(struct GlobalState* state);
void render_board(struct GlobalState* state, SDL_Renderer* renderer);
struct Vector2 get_cell_uv(int cell);

struct Matrix4 rotate_matrix3(struct Matrix4 mat);
struct Matrix4 rotate_matrix4(struct Matrix4 mat);

void rotate_shape(struct Shape* shape, int dir); // 1: clockwise, -1 counter-clockwise
