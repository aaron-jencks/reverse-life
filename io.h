#ifndef IO_H
#define IO_H

#include <stdint.h>

#include "grid.h"
#include "screen.h"
#include "types.h"

void copy_into_center(uint8_t** dest, coord_t dsize, uint8_t** src, coord_t ssize);
void draw_grid(grid_t current, grid_t previous, coord_t term);

#endif