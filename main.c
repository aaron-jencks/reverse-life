#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "grid.h"
#include "io.h"
#include "screen.h"
#include "types.h"


int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s inputfile frames\n", argv[0]);
        return 1;
    }

    // 1. Get terminal size once
    coord_t term = get_terminal_size();

    // 2. Load initial input
    grid_t input = load_grid(argv[1]);
    size_t frames;
    sscanf(argv[2], "%zd", &frames);

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


