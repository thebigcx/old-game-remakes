#include "board.h"
#include "global.h"

#include <SDL2/SDL_image.h>

SDL_Texture* cell_textures;
struct Shape shapes[8];
clock_t timer;
float speed = 10;

void init_textures(SDL_Renderer* renderer)
{
    SDL_Surface* surface = IMG_Load("assets/blocks.png");
    cell_textures = SDL_CreateTextureFromSurface(renderer, surface);
}

void setup_board(struct GlobalState* state)
{
    for (int x = 0; x < BOARD_WIDTH; x++)
    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        state->board.cells[x][y] = NO_BLOCK;
    }

    int s0[4][4] = {{0}}; // Empty
    shapes[0].type = NO_BLOCK;

    int s1[4][4] =
    {
        { 0, 0, 0, 0 },
        { 1, 1, 1, 1 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 }
    }; // I-Block 4x4
    shapes[1].mat_size = 4;
    shapes[1].type = I_BLOCK;

    int s2[4][4] =
    {
        { 1, 0, 0, 0 },
        { 1, 1, 1, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 }
    }; // J-block 3x3
    shapes[2].mat_size = 3;
    shapes[2].type = J_BLOCK;

    int s3[4][4] =
    {
        { 0, 0, 1, 0 },
        { 1, 1, 1, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 }
    }; // L-block 3x3
    shapes[3].mat_size = 3;
    shapes[3].type = L_BLOCK;

    int s4[4][4] =
    {
        { 1, 1, 0, 0 },
        { 1, 1, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 }
    }; // O-block 2x2
    shapes[4].mat_size = 2;
    shapes[4].type = O_BLOCK;

    int s5[4][4] =
    {
        { 0, 1, 1, 0 },
        { 1, 1, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 }
    }; // S-block 3x3
    shapes[5].mat_size = 3;
    shapes[5].type = S_BLOCK;

    int s6[4][4] =
    {
        { 0, 1, 0, 0 },
        { 1, 1, 1, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 }
    }; // T-block 3x3
    shapes[6].mat_size = 3;
    shapes[6].type = T_BLOCK;

    int s7[4][4] =
    {
        { 1, 1, 0, 0 },
        { 0, 1, 1, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 }
    }; // Z-block 3x3
    shapes[7].mat_size = 3;
    shapes[7].type = Z_BLOCK;

    memcpy(shapes[0].matrix.cells, s0, sizeof(s0));
    memcpy(shapes[1].matrix.cells, s1, sizeof(s1));
    memcpy(shapes[2].matrix.cells, s2, sizeof(s2));
    memcpy(shapes[3].matrix.cells, s3, sizeof(s3));
    memcpy(shapes[4].matrix.cells, s4, sizeof(s4));
    memcpy(shapes[5].matrix.cells, s5, sizeof(s5));
    memcpy(shapes[6].matrix.cells, s6, sizeof(s6));
    memcpy(shapes[7].matrix.cells, s7, sizeof(s7));

    state->current_shape = shapes[I_BLOCK];
    state->current_shape.x = 4;
}

void place_block(struct GlobalState* state)
{
    for (int x = 0; x < state->current_shape.mat_size; x++)
    for (int y = 0; y < state->current_shape.mat_size; y++)
    {
        if (!state->current_shape.matrix.cells[x][y])
            continue;

        int bx = state->current_shape.x + x;
        int by = state->current_shape.y + y;

        state->board.cells[bx][by] = state->current_shape.type;
    }

    // Select new shape
    int idx = (rand() % 7) + 1;
    state->current_shape = shapes[idx];
    state->current_shape.x = 4;
    if (block_colliding(state))
    {
        state->game_over = true;
    }

    // Check for line clears
    // Loop through each row and check if it is filled
    int lines_cleared = 0;
    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        bool filled = true;
        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            if (state->board.cells[x][y] == NO_BLOCK)
            {
                filled = false;
            }
        }

        if (filled)
        {
            clear_line(state, y);
            lines_cleared++;
        }
    }



    if (lines_cleared)
    {
        switch (lines_cleared)
        {
            case 1: state->score += 40; break;
            case 2: state->score += 100; break;
            case 3: state->score += 300; break;
            case 4: state->score += 1200; break;
        };

        printf("Current score: %d\n", state->score);
    }

}

// Clear line and move lines above down
void clear_line(struct GlobalState* state, int line)
{
    // Remove line
    for (int x = 0; x < BOARD_WIDTH; x++)
    {
        state->board.cells[x][line] = NO_BLOCK;
    }

    // Move lines down
    for (int x = 0; x < BOARD_WIDTH; x++)
    {
        for (int y = line; y > 0; y--)
        {
            state->board.cells[x][y] = state->board.cells[x][y - 1];
        }
    }
}

void update(struct GlobalState* state)
{
    if (clock() - timer > CLOCKS_PER_SEC / speed)
    {
        speed += 0.1f;
        state->current_shape.y++;
        if (block_colliding(state))
        {
            state->current_shape.y--;
            place_block(state);
        }
        timer = clock();
    }
}

void render_board(struct GlobalState* state, SDL_Renderer* renderer)
{
    for (int x = 0; x < BOARD_WIDTH; x++)
    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        if (state->board.cells[x][y] == NO_BLOCK)
            continue;

        SDL_Rect src, dst;

        struct Vector2 uv = get_cell_uv(state->board.cells[x][y]);
        src.x = uv.x;
        src.y = uv.y;
        src.w = 16;
        src.h = 16;

        dst.x = x * CELL_R_SIZE;
        dst.y = y * CELL_R_SIZE;
        dst.w = CELL_R_SIZE;
        dst.h = CELL_R_SIZE;

        SDL_RenderCopy(renderer, cell_textures, &src, &dst);
    }

    // Current piece

    for (int x = 0; x < state->current_shape.mat_size; x++)
    for (int y = 0; y < state->current_shape.mat_size; y++)
    {
        if (state->current_shape.matrix.cells[x][y] == 0)
            continue;

        SDL_Rect src, dst;

        struct Vector2 uv = get_cell_uv(state->current_shape.type);
        src.x = uv.x;
        src.y = uv.y;
        src.w = 16;
        src.h = 16;

        dst.x = (x * CELL_R_SIZE) + (state->current_shape.x * CELL_R_SIZE);
        dst.y = (y * CELL_R_SIZE) + (state->current_shape.y * CELL_R_SIZE);
        dst.w = CELL_R_SIZE;
        dst.h = CELL_R_SIZE;

        SDL_RenderCopy(renderer, cell_textures, &src, &dst);
    }
}

struct Vector2 get_cell_uv(int cell)
{
    switch (cell)
    {
        case 1:
        case 2:
        case 3:
        case 4:
            return (struct Vector2){ (cell - 1) * 16, 0 };
        case 5:
        case 6:
        case 7:
            return (struct Vector2){ (cell - 5) * 16, 16 };

        default:
            return (struct Vector2){ 0, 0 };
    }
}

void key_press(struct GlobalState* state, SDL_KeyboardEvent key)
{
    if (key.keysym.sym == SDLK_UP)
    {
        rotate_shape(&(state->current_shape), 1);
        if (block_colliding(state))
            rotate_shape(&(state->current_shape), -1);
    }
    else if (key.keysym.sym == SDLK_LEFT)
    {
        state->current_shape.x--;
        if (block_colliding(state))
            state->current_shape.x++;
    }
    else if (key.keysym.sym == SDLK_RIGHT)
    {
        state->current_shape.x++;
        if (block_colliding(state))
            state->current_shape.x--;
    }
    else if (key.keysym.sym == SDLK_DOWN)
    {
        state->current_shape.y++;
        if (block_colliding(state))
            state->current_shape.y--;
    }
    else if (key.keysym.sym == SDLK_SPACE)
    {
        while (!block_colliding(state))
        {
            state->current_shape.y++;
        }

        state->current_shape.y--; // Will have gone too far
        place_block(state);
    }
}

struct Matrix4 rotate_matrix3(struct Matrix4 mat)
{
    struct Matrix4 ret;
    memset(ret.cells, 0, sizeof(ret.cells));

    for (int x = 0; x < 3; x++)
    for (int y = 0; y < 3; y++)
    {
        ret.cells[x][y] = mat.cells[3 - y - 1][x];
    }

    return ret;
}

struct Matrix4 rotate_matrix4(struct Matrix4 mat)
{
    struct Matrix4 ret;
    memset(ret.cells, 0, sizeof(ret.cells));

    for (int x = 0; x < 4; x++)
    for (int y = 0; y < 4; y++)
    {
        ret.cells[x][y] = mat.cells[4 - y - 1][x];
    }

    return ret;
}

void rotate_shape(struct Shape* shape, int dir)
{
    // Ignore O-shape
    if (shape->mat_size == 3)
    {
        if (dir == -1)
            shape->matrix = rotate_matrix3(shape->matrix);
        else if (dir == 1)
        {
            for (int i = 0; i < 3; i++) shape->matrix = rotate_matrix3(shape->matrix); // MASSIVE HACK!
        }
    }
    else if (shape->mat_size == 4)
    {
        if (dir == -1)
            shape->matrix = rotate_matrix4(shape->matrix);
        if (dir == 1)
        {
            for (int i = 0; i < 3; i++) shape->matrix = rotate_matrix4(shape->matrix); // MASSIVE HACK!
        }
    }
}

bool block_colliding(struct GlobalState* state)
{
    for (int x = 0; x < state->current_shape.mat_size; x++)
    for (int y = 0; y < state->current_shape.mat_size; y++)
    {
        int bx = x + state->current_shape.x; // Board position
        int by = y + state->current_shape.y;


        int mat = state->current_shape.matrix.cells[x][y];

        if (mat && (bx < 0 || bx >= BOARD_WIDTH || by < 0 || by >= BOARD_HEIGHT))
            return true;

        int board = state->board.cells[bx][by];

        if (mat == 1 && board != NO_BLOCK)
        {
            return true;
        }


    }

    return false;
}
