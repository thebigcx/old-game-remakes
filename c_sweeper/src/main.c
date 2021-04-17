#include <stdio.h>
#include <SDL2/SDL.h>
#include <time.h>
#include <stdlib.h>

#include "bool.h"
#include "global.h"

struct GlobalState state;

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Minesweeper", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 500, 500, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    srand(time(NULL));

    state.game_over = false;

    init_tile_texture_atlas(renderer);
    generate_board(&state);

    state.time = clock();

    bool is_running = true;
    SDL_Event e;
    while (is_running)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
                is_running = false;

            if (e.type == SDL_MOUSEBUTTONDOWN && !state.game_over)
            {
                if (e.button.button == SDL_BUTTON_LEFT)
                {
                    on_click(&state, e.button.x, e.button.y, true);
                }
                else if (e.button.button == SDL_BUTTON_RIGHT)
                {
                    on_click(&state, e.button.x, e.button.y, false);
                }

            }
        }

        SDL_RenderClear(renderer);

        render_tiles(renderer, &state);

        SDL_RenderPresent(renderer);

        if (state.game_over)
        {
            if (state.did_complete)
            {
                printf("Map completed! Time: %f\n", (double)(clock() - state.time) / CLOCKS_PER_SEC);
            }
            else
            {
                printf("Game Over!\n");
            }

            while (1)
            {
                while (SDL_PollEvent(&e) != 0)
                {
                    if (e.type == SDL_QUIT)
                        return 0;
                }
            }
        }
    }

    SDL_DestroyTexture(tile_texture_atlas);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}
