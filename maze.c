#include "maze.h"

#define CELL_W 32
#define CELL_H 32

SDL_Color NOT_VISITED_COLOR = {100, 20, 150, 255};
SDL_Color VISITED_COLOR = {130, 50, 180, 255};
SDL_Color CURRENT_COLOR = {255, 20, 150, 255};
SDL_Color FONT_COLOR = { 30, 255, 30, 255};

enum 
{
    WALL_UP,
    WALL_DOWN,
    WALL_LEFT,
    WALL_RIGHT
};

// Structures

typedef struct configuration_s
{
    int nRows;
    int nCols;
} Configuration;

typedef struct cell_s
{
    int row;
    int col;

    int walls[4];

    int visited;
    SDL_Color color;
} Cell;

typedef struct maze_s
{
    int posX;
    int posY;

    int nRows;
    int nCols;

    int mouseHold;
    int mouseClickX;
    int mouseClickY;

    Cell **cells;
    Cell *currentCell;

    SDL_Renderer *renderer;
    TTF_Font *font;
    
    SDL_Texture *fontTexture;
    int fontTextureW;
    int fontTextureH;

    Configuration config;
} Maze;

// Private functions

void createTexture(Maze *maze)
{
    if(maze->fontTexture)
        SDL_DestroyTexture(maze->fontTexture);

    char text[30];

    sprintf(text, "Rows %d  |  Cols %d", maze->config.nRows, maze->config.nCols);

    SDL_Surface *fontSurface = TTF_RenderText_Solid(maze->font, text, FONT_COLOR);
    maze->fontTexture = SDL_CreateTextureFromSurface(maze->renderer, fontSurface);
    maze->fontTextureW = fontSurface->w;
    maze->fontTextureH = fontSurface->h;

    if(!fontSurface)
        printf("WARNING: could not create font texture\n");

    SDL_FreeSurface(fontSurface);
}

// Public functions

Maze *maze_create(SDL_Renderer *renderer)
{
    Maze *maze = malloc(sizeof(Maze));

    // Position && Dymension
    maze->posX = 100;
    maze->posY = 50;
    maze->nRows = 25;
    maze->nCols = 40;

    maze->mouseHold = 0;
    maze->mouseClickX = 0;
    maze->mouseClickY = 0;

    // Configuration
    maze->config.nRows = 25;
    maze->config.nCols = 40;

    // Font, Renderer && texture
    maze->renderer = renderer;
    maze->font = TTF_OpenFont("Ubuntu-L.ttf", 30);
    if(!maze->font)
        printf("WARNING: could not open font\n");
    else
    {
        maze->fontTexture = NULL;
        createTexture(maze);
    }


    // Cells
    maze->cells = malloc(maze->nRows * sizeof(Cell*));

    if(!maze->cells)
        return NULL;
    else
    {
        for (int i = 0; i < maze->nRows; i++)
        {
            maze->cells[i] = malloc(maze->nCols * sizeof(Cell));

            if(!maze->cells[i])
                return NULL;
            else
            {
                for(int j = 0; j < maze->nCols; j++)
                {
                    maze->cells[i][j].row = i;
                    maze->cells[i][j].col = j;
                    maze->cells[i][j].visited = 0;
                    maze->cells[i][j].color = NOT_VISITED_COLOR;

                    for(int k = 0; k < 4; k++)
                        maze->cells[i][j].walls[k] = 1;
                }
            }
        }

        maze->currentCell = &(maze->cells[0][0]);
        maze->currentCell->color = CURRENT_COLOR;
    }

    return maze;
}

void maze_destroy(Maze **maze)
{
    if(*maze != NULL)
    {
        if((*maze)->cells != NULL)
        {
            for(int i = 0; i < (*maze)->nRows; i++)
                free((*maze)->cells[i]);

            free((*maze)->cells);
        }

        if((*maze)->font)
            TTF_CloseFont((*maze)->font);
        
        if((*maze)->fontTexture)
            SDL_DestroyTexture((*maze)->fontTexture);

        free(*maze);
        *maze = NULL;
    }
}

void maze_render(Maze *maze)
{
    if(maze)
    {
        // Maze
        for(int i = 0; i < maze->nRows; i++)
        {
            for(int j = 0; j < maze->nCols; j++)
            {
                SDL_Rect cellRect = { j * CELL_W + maze->posX, i * CELL_H + maze->posY, CELL_W, CELL_H };

                SDL_SetRenderDrawColor(maze->renderer, maze->cells[i][j].color.r, maze->cells[i][j].color.g, maze->cells[i][j].color.b, maze->cells[i][j].color.a);
                SDL_RenderFillRect(maze->renderer, &cellRect);

                SDL_SetRenderDrawColor(maze->renderer, 0, 0, 0, 255);

                for(int k = 0; k < 4; k++)
                {
                    if(maze->cells[i][j].walls[k])
                        switch(k)
                        {
                            case WALL_UP:    SDL_RenderDrawLine(maze->renderer, cellRect.x             , cellRect.y             , cellRect.x + cellRect.w, cellRect.y             ); break;
                            case WALL_DOWN:  SDL_RenderDrawLine(maze->renderer, cellRect.x             , cellRect.y + cellRect.h, cellRect.x + cellRect.w, cellRect.y + cellRect.h); break;
                            case WALL_LEFT:  SDL_RenderDrawLine(maze->renderer, cellRect.x             , cellRect.y             , cellRect.x             , cellRect.y + cellRect.h); break;
                            case WALL_RIGHT: SDL_RenderDrawLine(maze->renderer, cellRect.x + cellRect.w, cellRect.y             , cellRect.x + cellRect.w, cellRect.y + cellRect.h); break;
                        }
                }
            }
        }

        // Font
        if(maze->fontTexture)
        {
            SDL_Rect destRect = { 10, 10, maze->fontTextureW, maze->fontTextureH };
            SDL_RenderCopy(maze->renderer, maze->fontTexture, NULL, &destRect);
        }
    }

    SDL_SetRenderDrawColor(maze->renderer, 51, 51, 51, 255);
}

void maze_handleInput(Maze *maze, SDL_Event *e)
{
    if(e->type == SDL_KEYDOWN)
    {
        switch(e->key.keysym.sym)
        {
            case SDLK_UP:    maze->config.nRows++; break;
            case SDLK_RIGHT: maze->config.nCols++; break;

            case SDLK_DOWN:
                maze->config.nRows--;
                if(maze->config.nRows < 2) maze->config.nRows = 2;
                break;
                
            case SDLK_LEFT:
                maze->config.nCols--;
                if(maze->config.nCols < 2) maze->config.nCols = 2;
                break;
        }
    }

    if(e->type == SDL_MOUSEBUTTONDOWN && e->button.button == 1)
    {
        maze->mouseHold = 1;
        maze->mouseClickX = e->button.x;
        maze->mouseClickY = e->button.y;
    }

    if(e->type == SDL_MOUSEBUTTONUP && e->button.button == 1)
        maze->mouseHold = 0;

    if(maze->mouseHold)
    {
        maze->posX += e->button.x - maze->mouseClickX;
        maze->posY += e->button.y - maze->mouseClickY;

        maze->mouseClickX = e->button.x;
        maze->mouseClickY = e->button.y;
    }
}

void maze_generate(Maze *maze)
{
    printf("plop\n");
}
