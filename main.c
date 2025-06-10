#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "grid.h"
#include "screen.h"


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

void draw_grid(grid_t current, grid_t previous, coord_t term) {
    coord_t cc;
    for (size_t i = 0; i < current.size.y && i < term.y; i++) {
        for (size_t j = 0; j < current.size.x && j < term.x; j++) {
            if (current.grid[i][j] != previous.grid[i][j]) {
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

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s inputfile\n", argv[0]);
        return 1;
    }

    // 1. Get terminal size once
    coord_t term = get_terminal_size();

    // 2. Load initial input
    grid_t input = load_grid(argv[1]);

    // 3. Allocate full-screen field
    grid_t current = create_grid(term);

    copy_into_center(current.grid, current.size,
                     input.grid, input.size);
    destroy_grid(input);

    // 4. Allocate previous grid
    grid_t previous = copy_grid(current);

    clear_screen();

    // 5. Simulation loop
    while (true) {
        draw_grid(current, previous, term);
        usleep(100000);
        step_grid(current);
    }

    return 0;
}


