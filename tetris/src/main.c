#include <stdio.h>
#include <SDL2/SDL.h>

#include "bool.h"
#include "global.h"

struct GlobalState state;

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    srand(time(NULL));

    SDL_Window* window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 400, 880, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    init_textures(renderer);
    setup_board(&state);

    bool is_running = true;
    SDL_Event e;
    timer = clock();
    while (is_running)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                is_running = false;

            if (e.type == SDL_KEYDOWN)
            {
                key_press(&state, e.key);
            }
        }

        SDL_RenderClear(renderer);

        update(&state);
        render_board(&state, renderer);

        SDL_RenderPresent(renderer);
    }

    return 0;
}
