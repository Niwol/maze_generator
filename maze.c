#include "maze.h"

SDL_Color NOT_VISITED_COLOR = {100, 20, 150, 255};
SDL_Color VISITED_COLOR = {130, 50, 180, 255};
SDL_Color CURRENT_COLOR = {255, 20, 150, 255};
SDL_Color FONT_COLOR = { 30, 255, 30, 255};

enum 
{
    WALL_UP,
    WALL_LEFT,
    WALL_DOWN,
    WALL_RIGHT
};

// Structures

typedef struct configuration_s
{
    int nRows;
    int nCols;

    int cellW;
    int cellH;
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

    int cellW;
    int cellH;

    int nRows;
    int nCols;

    int mouseHold;
    int mouseClickX;
    int mouseClickY;

    Cell **cells;
    Cell *currentCell;

    Stack *stack;

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

    char text[200];

    sprintf(text, "Rows %d  |  Cols %d  | Cell: W = %d ; H = %d  |  Maze pos (%d , %d)", maze->config.nRows, maze->config.nCols, maze->config.cellW, maze->config.cellH, maze->posX, maze->posY);

    SDL_Surface *fontSurface = TTF_RenderText_Solid(maze->font, text, FONT_COLOR);
    maze->fontTexture = SDL_CreateTextureFromSurface(maze->renderer, fontSurface);
    maze->fontTextureW = fontSurface->w;
    maze->fontTextureH = fontSurface->h;

    if(!fontSurface)
        printf("WARNING: could not create font texture\n");

    SDL_FreeSurface(fontSurface);
}

void allocGrid(Maze *maze)
{
    maze->cells = malloc(maze->config.nRows * sizeof(Cell*));

    for(int i = 0; i < maze->config.nRows; i++)
    {
        maze->cells[i] = malloc(maze->config.nCols * sizeof(Cell));
        
        for(int j = 0; j < maze->config.nCols; j++)
        {
            maze->cells[i][j].row = i;
            maze->cells[i][j].col = j;
            maze->cells[i][j].visited = 0;
            maze->cells[i][j].color = NOT_VISITED_COLOR;
            
            for(int k = 0; k < 4; k++)
                maze->cells[i][j].walls[k] = 1;
        }
    }

    maze->nRows = maze->config.nRows;
    maze->nCols = maze->config.nCols;

    maze->cellW = maze->config.cellW;
    maze->cellH = maze->config.cellH;

    maze->currentCell = &(maze->cells[0][0]);
    maze->currentCell->color = CURRENT_COLOR;
    maze->currentCell->visited = 1;
}

// Public functions

Maze *maze_create(SDL_Renderer *renderer, int nRows, int nCols, int cellW, int cellH)
{
    Maze *maze = malloc(sizeof(Maze));

    // Position && Dymension
    maze->posX = 100;
    maze->posY = 50;
    maze->cellW = cellW;
    maze->cellH = cellH;
    maze->nRows = nRows;
    maze->nCols = nCols;

    maze->mouseHold = 0;
    maze->mouseClickX = 0;
    maze->mouseClickY = 0;

    // Configuration
    maze->config.nRows = nRows;
    maze->config.nCols = nCols;
    maze->config.cellW = cellW;
    maze->config.cellH = cellH;

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
    allocGrid(maze);

    maze->stack = stack_create();

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

        stack_destroy(&((*maze)->stack));

        free(*maze);
        *maze = NULL;
    }
}

void maze_render(Maze *maze)
{
    if(maze)
    {
        // Maze
        int begX = -(maze->posX / maze->cellW);
        int begY = -(maze->posY / maze->cellH);

        int endX = begX + 1500 / maze->cellW + 1;
        int endY = begY + 900 / maze->cellH + 1;

        if(begX < 0) begX = 0;
        if(begY < 0) begY = 0;

        if(endX > maze->nCols) endX = maze->nCols;
        if(endY > maze->nRows) endY = maze->nRows;

        for(int i = begY; i < endY; i++)
        {
            for(int j = begX; j < endX - 1; j++)
            {
                SDL_Rect cellRect = { j * maze->cellW + maze->posX, i * maze->cellH + maze->posY, maze->cellW, maze->cellH };

                SDL_SetRenderDrawColor(maze->renderer, maze->cells[i][j].color.r, maze->cells[i][j].color.g, maze->cells[i][j].color.b, maze->cells[i][j].color.a);
                SDL_RenderFillRect(maze->renderer, &cellRect);

                SDL_SetRenderDrawColor(maze->renderer, 0, 0, 0, 255);

                
                for(int k = 0; k < 2; k++)
                {
                    if(maze->cells[i][j].walls[k])
                        switch(k)
                        {
                            case WALL_UP:    SDL_RenderDrawLine(maze->renderer, cellRect.x             , cellRect.y             , cellRect.x + cellRect.w, cellRect.y             ); break;
                            //case WALL_DOWN:  SDL_RenderDrawLine(maze->renderer, cellRect.x             , cellRect.y + cellRect.h, cellRect.x + cellRect.w, cellRect.y + cellRect.h); break;
                            case WALL_LEFT:  SDL_RenderDrawLine(maze->renderer, cellRect.x             , cellRect.y             , cellRect.x             , cellRect.y + cellRect.h); break;
                            //case WALL_RIGHT: SDL_RenderDrawLine(maze->renderer, cellRect.x + cellRect.w, cellRect.y             , cellRect.x + cellRect.w, cellRect.y + cellRect.h); break;
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

            case SDLK_z: maze->config.cellH++; break;
            case SDLK_d: maze->config.cellW++; break;

            case SDLK_s:
                maze->config.cellH--;
                if(maze->config.cellH < 5)maze->config.cellH = 5;
                break;

            case SDLK_q:
                maze->config.cellW--;
                if(maze->config.cellW < 5)maze->config.cellW = 5;
                break;
        }

        createTexture(maze);
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
    if(maze)
    {
        if(stack_size(maze->stack) == 0)
        {
            for(int i = 0; i < maze->nRows; i++)
                free(maze->cells[i]);
            
            free(maze->cells);

            allocGrid(maze);

            stack_push(maze->stack, maze->currentCell);
        }

        SDL_Event e;
        int stepMode = 1;
        int step = 0;

        int quit = 0;

        while(stack_size(maze->stack) && !quit)
        {
            while(SDL_PollEvent(&e))
            {
                if(e.type == SDL_QUIT)
                    exit(0);

                if(e.type == SDL_KEYDOWN)
                {
                    switch(e.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            quit = 1;
                            break;

                        case SDLK_m: stepMode = !stepMode; break;
                        case SDLK_n: step = 1; break;
                    }
                }

                maze_handleInput(maze, &e);
            }

            if(step || !stepMode)
            {
                int nonVisitedNeighbors = 0;
                Cell *posibleCells[4] = { NULL };

                if(maze->currentCell->row != 0 && maze->cells[maze->currentCell->row - 1][maze->currentCell->col].visited == 0)
                {
                    posibleCells[nonVisitedNeighbors] = &(maze->cells[maze->currentCell->row - 1][maze->currentCell->col]);
                    nonVisitedNeighbors += 1;
                }

                if(maze->currentCell->row != maze->nRows - 1 && maze->cells[maze->currentCell->row + 1][maze->currentCell->col].visited == 0)
                {
                    posibleCells[nonVisitedNeighbors] = &(maze->cells[maze->currentCell->row + 1][maze->currentCell->col]);
                    nonVisitedNeighbors += 1;
                }

                if(maze->currentCell->col != 0 && maze->cells[maze->currentCell->row][maze->currentCell->col - 1].visited == 0)
                {
                    posibleCells[nonVisitedNeighbors] = &(maze->cells[maze->currentCell->row][maze->currentCell->col - 1]);
                    nonVisitedNeighbors += 1;
                }

                if(maze->currentCell->col != maze->nCols - 1 && maze->cells[maze->currentCell->row][maze->currentCell->col + 1].visited == 0)
                {
                    posibleCells[nonVisitedNeighbors] = &(maze->cells[maze->currentCell->row][maze->currentCell->col + 1]);
                    nonVisitedNeighbors += 1;
                }

                if(nonVisitedNeighbors)
                {
                    int randomCell = rand() % nonVisitedNeighbors;
                    Cell *nextCell = posibleCells[randomCell];
                    Cell *prevCell = maze->currentCell;

                    // Open Walls
                    if(maze->currentCell->row == nextCell->row)
                    {
                        if(maze->currentCell->col - nextCell->col == 1) { maze->currentCell->walls[WALL_LEFT] = 0; nextCell->walls[WALL_RIGHT] = 0; }
                        else                                            { maze->currentCell->walls[WALL_RIGHT] = 0; nextCell->walls[WALL_LEFT] = 0; }
                    }
                    else
                    {
                        if(maze->currentCell->row - nextCell->row == 1) { maze->currentCell->walls[WALL_UP] = 0; nextCell->walls[WALL_DOWN] = 0; }
                        else                                            { maze->currentCell->walls[WALL_DOWN] = 0; nextCell->walls[WALL_UP] = 0; }
                    }

                    // Setting next cell
                    maze->currentCell = nextCell;

                    // Setting colors
                    maze->currentCell->visited = 1;
                    maze->currentCell->color = CURRENT_COLOR;
                    prevCell->color = VISITED_COLOR;

                    // Push cell on the stack
                    stack_push(maze->stack, maze->currentCell);
                }
                else
                {
                    Cell *prevCell = maze->currentCell;
                    maze->currentCell = stack_top(maze->stack);
                    stack_pop(maze->stack);

                    prevCell->color = VISITED_COLOR;
                    maze->currentCell->color = CURRENT_COLOR;
                }

                step = 0;
            }

            // Rendering
            SDL_SetRenderDrawColor(maze->renderer, 51, 51, 51, 255);
            SDL_RenderClear(maze->renderer);

            maze_render(maze);

            SDL_RenderPresent(maze->renderer);
        }
    }
}
