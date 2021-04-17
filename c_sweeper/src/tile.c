#include "tile.h"
#include "global.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>

SDL_Texture* tile_texture_atlas;

int get_adj_mines(struct GlobalState* state, int x, int y)
{
    int count = 0;

    if (x > 0 && y > 0)
        count += state->tiles[x - 1][y - 1].mine ? 1 : 0;

    if (x > 0)
        count += state->tiles[x - 1][y].mine ? 1 : 0;

    if (y > 0)
        count += state->tiles[x][y - 1].mine ? 1 : 0;

    if (x > 0 && y < BOARD_SIZE - 1)
        count += state->tiles[x - 1][y + 1].mine ? 1 : 0;

    if (x < BOARD_SIZE - 1 && y > 0)
        count += state->tiles[x + 1][y - 1].mine ? 1 : 0;

    if (x < BOARD_SIZE - 1 && y < BOARD_SIZE - 1)
        count += state->tiles[x + 1][y + 1].mine ? 1 : 0;

    if (x < BOARD_SIZE - 1)
        count += state->tiles[x + 1][y].mine ? 1 : 0;

    if (y < BOARD_SIZE - 1)
        count += state->tiles[x][y + 1].mine ? 1 : 0;

    return count;
}

void reveal(struct GlobalState* state, int x, int y)
{
    if (x < 0 || x > BOARD_SIZE - 1 || y < 0 || y > BOARD_SIZE - 1)
        return;

    state->tiles[x][y].hidden = false;
    if (state->tiles[x][y].mine)
    {
        state->game_over = true;
        state->did_complete = false;
        state->tiles[x][y].mine_clicked = true;
        reveal_mines(state);
        return;
    }
    if (!state->tiles[x][y].recurse && state->tiles[x][y].num_adj_mines == 0 && !state->tiles[x][y].mine)
    {
        state->tiles[x][y].recurse = true;
        reveal_adjacents(state, x, y);
    }
}

void reveal_adjacents(struct GlobalState* state, int x, int y)
{
    reveal(state, x - 1, y - 1);
    reveal(state, x - 1, y    );
    reveal(state, x,     y - 1);
    reveal(state, x - 1, y + 1);
    reveal(state, x + 1, y - 1);
    reveal(state, x + 1, y + 1);
    reveal(state, x + 1, y    );
    reveal(state, x,     y + 1);
}

static bool is_flagged(struct GlobalState* state, int x, int y)
{
    if (x < 0 || x > BOARD_SIZE - 1 || y < 0 || y > BOARD_SIZE - 1)
        return false;

    return state->tiles[x][y].flagged;
}

void auto_reveal_adjacents(struct GlobalState* state, int x, int y)
{
    int flags = 0;

    flags += is_flagged(state, x - 1, y);
    flags += is_flagged(state, x + 1, y);
    flags += is_flagged(state, x - 1, y - 1);
    flags += is_flagged(state, x + 1, y + 1);
    flags += is_flagged(state, x, y - 1);
    flags += is_flagged(state, x, y + 1);
    flags += is_flagged(state, x + 1, y - 1);
    flags += is_flagged(state, x - 1, y + 1);

    if (flags != state->tiles[x][y].num_adj_mines)
    {
        return;
    }

    if (!is_flagged(state, x - 1, y)) reveal(state, x - 1, y);
    if (!is_flagged(state, x + 1, y)) reveal(state, x + 1, y);
    if (!is_flagged(state, x - 1, y - 1)) reveal(state, x - 1, y - 1);
    if (!is_flagged(state, x + 1, y + 1)) reveal(state, x + 1, y + 1);
    if (!is_flagged(state, x, y - 1)) reveal(state, x, y - 1);
    if (!is_flagged(state, x, y + 1)) reveal(state, x, y + 1);
    if (!is_flagged(state, x + 1, y - 1)) reveal(state, x + 1, y - 1);
    if (!is_flagged(state, x - 1, y + 1)) reveal(state, x - 1, y + 1);
}

struct Vector2 get_tile_uv(struct Tile* tile)
{
    if (tile->incorrect_flag)
        return (struct Vector2){ 80, 0 };

    if (tile->hidden)
    {
        if (tile->flagged)
            return (struct Vector2){ 32, 0 };
        else
            return (struct Vector2){ 0, 0 };
    }
    else
    {
        if (tile->mine)
            if (tile->mine_clicked)
                return (struct Vector2){ 64, 0 };
            else
                return (struct Vector2){ 48, 0 };
        else
        {
            switch (tile->num_adj_mines)
            {
                case 0: return (struct Vector2){ 16, 0 };
                case 1: return (struct Vector2){ 96, 0 };
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9: return (struct Vector2){ (tile->num_adj_mines - 2) * 16, 16 };
            }
        }
    }
}

void on_click(struct GlobalState* state, int x, int y, bool is_left_click)
{
    int tx = floor((float)x / (float)TILE_R_SIZE);
    int ty = floor((float)y / (float)TILE_R_SIZE);

    assert(tx >= 0 && ty >= 0 && tx < BOARD_SIZE && ty < BOARD_SIZE);

    if (is_left_click)
    {
        if (!is_flagged(state, tx, ty))
        {
            reveal(state, tx, ty);
        }
    }
    else
    {
        if (!state->tiles[tx][ty].hidden)
        {
            // Check both buttons
            if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT))
            {
                auto_reveal_adjacents(state, tx, ty);
            }
        }

        if (state->tiles[tx][ty].hidden && !state->tiles[tx][ty].flagged)
        {
            state->tiles[tx][ty].flagged = true;
            state->flags++;
            printf("Remaining mines: %d\n", MINE_COUNT - state->flags);
        }
        else if (state->tiles[tx][ty].flagged)
        {
            // Remove flag
            state->tiles[tx][ty].flagged = false;
            state->flags--;
        }
    }

    check_completion(state);
}

void reveal_mines(struct GlobalState* state)
{
    for (int x = 0; x < BOARD_SIZE; x++)
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        if (state->tiles[x][y].mine && !state->tiles[x][y].flagged)
        {
            state->tiles[x][y].hidden = false;
        }
        if (state->tiles[x][y].flagged && !state->tiles[x][y].mine)
        {
            state->tiles[x][y].incorrect_flag = true;
        }
    }
}

static bool is_mine(struct GlobalState* state, int x, int y)
{
    return state->tiles[x][y].mine;
}

void generate_board(struct GlobalState* state)
{
    // Init values
    for (int x = 0; x < BOARD_SIZE; x++)
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        state->tiles[x][y].hidden = true;
        state->tiles[x][y].flagged = false;
        state->tiles[x][y].num_adj_mines = 0;

        state->tiles[x][y].mine = false;
        state->tiles[x][y].mine_clicked = false;
        state->tiles[x][y].incorrect_flag = false;
        state->tiles[x][y].recurse = false;
    }

    // Set mines
    for (int i = 0; i < MINE_COUNT; i++)
    {
        int x, y;
        do // WOW! A good use of do-while!
        {
            x = (rand() % BOARD_SIZE);
            y = (rand() % BOARD_SIZE);
        }
        while (is_mine(state, x, y));

        state->tiles[x][y].mine = true;
    }

    // Generate mine counts
    for (int x = 0; x < BOARD_SIZE; x++)
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        state->tiles[x][y].num_adj_mines = get_adj_mines(state, x, y);
    }
}

void init_tile_texture_atlas(SDL_Renderer* renderer)
{
    SDL_Surface* surface = IMG_Load("assets/tiles.png");
    tile_texture_atlas = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

void render_tiles(SDL_Renderer* renderer, struct GlobalState* state)
{
    for (int x = 0; x < BOARD_SIZE; x++)
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        SDL_Rect src, dst;

        dst.x = x * TILE_R_SIZE;
        dst.y = y * TILE_R_SIZE;
        dst.w = TILE_R_SIZE;
        dst.h = TILE_R_SIZE;

        struct Vector2 uv_coord = get_tile_uv(&state->tiles[x][y]);

        src.x = uv_coord.x;
        src.y = uv_coord.y;
        src.w = 16;
        src.h = 16;

        SDL_RenderCopy(renderer, tile_texture_atlas, &src, &dst);
    }
}

void check_completion(struct GlobalState* state)
{
    bool unflagged_not_mine = false;
    bool unflagged_mine = false;

    for (int x = 0; x < BOARD_SIZE; x++)
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        if (state->tiles[x][y].hidden && !state->tiles[x][y].mine)
        {
            unflagged_not_mine = true;
        }
        if (state->tiles[x][y].mine && !state->tiles[x][y].flagged)
        {
            unflagged_mine = true;
        }
    }

    if (unflagged_mine && unflagged_not_mine)
        return;

    state->game_over = true;
    state->did_complete = true;
}
