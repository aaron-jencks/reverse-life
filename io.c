#include "io.h"

#include <stddef.h>
#include <stdio.h>


void copy_into_center(uint8_t** dest, coord_t dsize,
                      uint8_t** src, coord_t ssize) {
    size_t row_offset = (dsize.y > ssize.y) ? (dsize.y - ssize.y) / 2 : 0;
    size_t col_offset = (dsize.x > ssize.x) ? (dsize.x - ssize.x) / 2 : 0;

    for (size_t i = 0; i < ssize.y && (row_offset + i) < dsize.y; i++) {
        for (size_t j = 0; j < ssize.x && (col_offset + j) < dsize.x; j++) {
            dest[row_offset + i][col_offset + j] = src[i][j];
        }
    }
}

void draw_grid(grid_t current, grid_t previous, coord_t term, bool force_alive) {
    coord_t cc;
    for (size_t i = 0; i < current.size.y && i < term.y; i++) {
        for (size_t j = 0; j < current.size.x && j < term.x; j++) {
            if (current.grid[i][j] != previous.grid[i][j] || (force_alive && current.grid[i][j] > 0)) {
                cc.x = j;
                cc.y = i;
                cursor_move(cc);
                putchar(current.grid[i][j] ? '*' : ' ');
                previous.grid[i][j] = current.grid[i][j];
            }
        }
    }
    fflush(stdout);
}