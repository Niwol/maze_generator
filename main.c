#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "stack.h"
#include "maze.h"

#define SCREEN_W 1500
#define SCREEN_H 900

int init(SDL_Window **w, SDL_Renderer **r);
void testStack();

int main()
{
    SDL_Window *window;
    SDL_Renderer *renderer;

    int quit = 0;

    if(!init(&window, &renderer))
    {
        printf("FAIL: init\n");
        exit(1);
    }
    else
    {
        SDL_Event e;
        Maze *maze = maze_create(renderer);

        while(!quit)
        {
            while(SDL_PollEvent(&e))
            {
                if(e.type == SDL_QUIT)
                    quit = 1;
                
                if(e.type == SDL_KEYDOWN)
                {
                    switch(e.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            quit = 1;
                            break;

                        case SDLK_g:
                            maze_generate(maze);
                    }
                }

                maze_handleInput(maze, &e);
            }

            SDL_SetRenderDrawColor(renderer, 51, 51, 51, 255);
            SDL_RenderClear(renderer);

            maze_render(maze);

            SDL_RenderPresent(renderer);
        }

        maze_destroy(&maze);

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        SDL_Quit();
        TTF_Quit();
    }

    return 0;
}

int init(SDL_Window **w, SDL_Renderer **r)
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    *w = SDL_CreateWindow("Maze Generator", SDL_WINDOWPOS_UNDEFINED_DISPLAY(0), SDL_WINDOWPOS_UNDEFINED_DISPLAY(0), SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN);
    if(!*w)
    {
        printf("FAIL: create window\n");
        return 0;
    }

    *r = SDL_CreateRenderer(*w, 0, SDL_RENDERER_ACCELERATED);
    if(!*r)
    {
        SDL_DestroyWindow(*w);
        printf("FAIL: create renderer\n");
        return 0;
    }

    return 1;
}

void testStack()
{
    Stack *s = stack_create();

    for(int i = 0; i < 10; i++)
    {
        int *n = malloc(sizeof(int));
        *n = i;
        stack_push(s, n);
    }

    printf("Stack content:\n");
    for(int i = 0; i < 10; i++)
    {
        int *n = stack_top(s);
        printf("%d ", *n);
        stack_pop(s);
        free(n);
    }
    printf("\n");

    stack_destroy(&s);
}
