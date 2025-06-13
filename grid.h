#ifndef GRID_H
#define GRID_H

#include <stdbool.h>
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

/**
 * Compresses the neighbors around a coordinate into a 8 bit integer
 * 
 * The integer has the following layout:
 * 0000 0000
 * ABCD EFGH
 * Where:
 * A B C
 * D . E
 * F G H
 */
uint8_t compress_neighbors(grid_t g, coord_t coordinate);
icoord_t decode_neighbor_bit(uint8_t state, size_t bit, coord_t origin);

/**
 * Determines the next state of a cell given the neighbors and it's current state
 */
bool get_state(uint8_t neighbors, bool state);

#endif