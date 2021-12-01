#include <SDL2/SDL.h>
#include <time.h>

#define TILESIZE 100
#define WIDTH 10
#define HEIGHT 10

#define SPEED 10

struct vector2
{
    int x, y;
};

struct vector2 *snake = NULL;
size_t snake_len = 3;

struct vector2 dir = { 1, 0 };
struct vector2 apple;

struct vector2 kqueue[16];
size_t kqueue_len = 0;

int running = 1;

clock_t timer = 0;

void new_apple()
{
retry:
    apple.x = ((float)rand() / RAND_MAX) * WIDTH;
    apple.y = ((float)rand() / RAND_MAX) * HEIGHT;

    for (size_t i = 0; i < snake_len; i++)
    {
        if (snake[i].x == apple.x && snake[i].y == apple.y)
            goto retry;
    }
}

void game_over()
{
    printf("Game over\nScore: %d\n", snake_len);

    SDL_Event e;
    while (1)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                running = 0;
                return;
            }
        }
    }
}

int main()
{
    timer = clock();

    snake = malloc(sizeof(struct vector2) * snake_len);

    snake[0] = (struct vector2) { 3, 0 };
    snake[1] = (struct vector2) { 2, 0 };
    snake[2] = (struct vector2) { 1, 0 };

    new_apple();
    
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Snake", 0, 0, TILESIZE * WIDTH, TILESIZE * HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    uint64_t frames = 0;

    SDL_Event e;
    while (running)
    {
        frames++;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                running = 0;
                break;
            }

            if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                    case SDLK_UP: kqueue[kqueue_len++] = (struct vector2) { 0, -1 }; break;
                    case SDLK_DOWN: kqueue[kqueue_len++] = (struct vector2) { 0, 1 }; break;
                    case SDLK_LEFT: kqueue[kqueue_len++] = (struct vector2) { -1, 0 }; break;
                    case SDLK_RIGHT: kqueue[kqueue_len++] = (struct vector2) { 1, 0 }; break;
                };

            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        for (size_t i = 0; i < snake_len; i++)
        {
            SDL_Rect rect = { snake[i].x * TILESIZE, snake[i].y * TILESIZE, TILESIZE, TILESIZE };
            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        
        SDL_Rect rect = { apple.x * TILESIZE, apple.y * TILESIZE, TILESIZE, TILESIZE };
        SDL_RenderFillRect(renderer, &rect);

        SDL_RenderPresent(renderer);

        if (clock() - timer > CLOCKS_PER_SEC / SPEED)
        {
            while (kqueue_len && kqueue_len--)
            {
                if (dir.x == -kqueue[0].x || dir.y == -kqueue[0].y)
                    continue;

                dir = kqueue[0];
            
                for (size_t i = 0; i < kqueue_len; i++)
                    kqueue[i] = kqueue[i + 1];
                break;
            }

            for (size_t i = snake_len - 1; i > 0; i--)
                snake[i] = snake[i - 1];
            
            snake[0].x += dir.x;
            snake[0].y += dir.y;

            for (size_t i = 0; i < snake_len; i++)
            {
                if (snake[i].x < 0 || snake[i].x >= WIDTH || snake[i].y < 0 || snake[i].y >= HEIGHT)
                    game_over();

                for (size_t j = 0; j < snake_len; j++)
                    if (i != j && snake[i].x == snake[j].x && snake[i].y == snake[j].y)
                        game_over(); 
            }

            if (snake[0].x == apple.x && snake[0].y == apple.y)
            {
                new_apple();
                snake = realloc(snake, sizeof(struct vector2) * ++snake_len);
                snake[snake_len - 1] = snake[snake_len - 2];
            }

            timer = clock();
        }
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    return 0;
};