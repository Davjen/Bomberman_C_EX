#ifndef BOMBERMAN_H
#define BOMBERMAN_H

#include <stdint.h>
#include <SDL.h>

#define BLOCK_MASK_UNWALKABLE 0x0100
#define BLOCK_MASK_TELEPORT 0x0200
#define BLOCK_MASK_EXIT 0x0400

#define BLOCK_GROUND 0
#define BLOCK_WALL 1
#define BLOCK_DESTROYABLE 2

typedef struct level
{
    int32_t *cells;     // content of the level
    uint32_t cols;      // number of columns of the level grid
    uint32_t rows;      // number of rows of the level grid
    uint32_t cell_size; // size in pixel of a single cell
} level_t;

typedef struct movable
{
    float x;
    float y;
    uint32_t width;
    uint32_t height;
    float speed;
} movable_t;

typedef struct drawable{

    SDL_Rect rect;
    SDL_Texture *texture;

}drawable_t;
// try moving an object on the level
int32_t move_on_level(level_t* level, movable_t* movable, const float delta_x, const float delta_y);

typedef struct bomberman
{
    
    movable_t movable;
    drawable_t drawable;
    uint32_t number_of_bombs;
    uint32_t dropped_bombs;
    uint32_t bomb_power;
    
} bomberman_t;

extern bomberman_t local_player;
extern bomberman_t remote_player;
extern bomberman_t player0;
extern bomberman_t player1;

// initialize a level structure
int level_init(level_t *level, const uint32_t cols, const uint32_t rows, const uint32_t cell_size, int32_t *cells);
// get the cell content at the specified coordinates
int32_t level_cell(level_t* level, const uint32_t col, const uint32_t row);

#endif