#ifndef MAZE_H
#define MAZE_H

#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "stack.h"

typedef struct maze_s Maze;

Maze *maze_create(SDL_Renderer *renderer, int nRows, int nCols, int cellW, int cellH);
void maze_destroy(Maze **maze);
void maze_render(Maze *maze);
void maze_handleInput(Maze *maze, SDL_Event *e);
void maze_generate(Maze *maze);

#endif // MAZE_H