#ifndef GRID_H
#define GRID_H

#include <stdint.h>

#include "types.h"


typedef struct {
    uint8_t** grid;
    coord_t size;
} grid_t;


typedef enum {
    DEAD = 0,
    ALIVE = 1,
    UNKNOWN = 2,
} States;

grid_t load_grid(const char* filename);

grid_t create_grid(coord_t size);
grid_t copy_grid(grid_t g);
void destroy_grid(grid_t g);
grid_t step_grid(grid_t g);

grid_t prior_grid(grid_t g);

#endif