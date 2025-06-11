#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grid.h"


grid_t load_grid(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("fopen");
        exit(1);
    }

    grid_t field;

    char buffer[1024];
    size_t capacity = 16;
    size_t row_count = 0;
    field.grid = malloc(capacity * sizeof(uint8_t*));
    field.size.x = 0;

    while (fgets(buffer, 1024, f)) {
        size_t len = strcspn(buffer, "\n");
        if (row_count >= capacity) {
            capacity *= 2;
            field.grid = realloc(field.grid, capacity * sizeof(uint8_t*));
        }
        if (field.size.x < len)
            field.size.x = len;

        field.grid[row_count] = calloc(len, sizeof(uint8_t));
        for (size_t i = 0; i < len; i++) {
            field.grid[row_count][i] = buffer[i] == '*' ? 1 : 0;
        }
        row_count++;
    }

    field.size.y = row_count;
    fclose(f);
    return field;
}

grid_t create_grid(coord_t size) {
    grid_t g;
    g.size = size;
    g.grid = malloc(size.y * sizeof(uint8_t*));
    for (size_t i = 0; i < size.y; i++) {
        g.grid[i] = calloc(size.x, sizeof(uint8_t));
    }
    return g;
}

grid_t copy_grid(grid_t g) {
    grid_t result = create_grid(g.size);
    for (size_t i = 0; i < g.size.y; i++) {
        for (size_t j = 0; j < g.size.x; j++) {
            result.grid[i][j] = g.grid[i][j];
        }
    }
    return result;
}

void destroy_grid(grid_t g) {
    for (size_t i = 0; i < g.size.y; i++)
        free(g.grid[i]);
    free(g.grid);
}

grid_t step_grid(grid_t g) {
    grid_t next = create_grid(g.size);
    for (size_t i = 0; i < g.size.y; i++) {
        for (size_t j = 0; j < g.size.x; j++) {
            int live_neighbors = 0;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) continue;
                    size_t ni = i + dx;
                    size_t nj = j + dy;
                    if (ni >= 0 && ni < g.size.y && nj >= 0 && nj < g.size.x)
                        live_neighbors += g.grid[ni][nj] > 0 ? 1 : 0;
                }
            }

            if (g.grid[i][j] > 0)
                next.grid[i][j] = ((live_neighbors < 4) && (live_neighbors >= 2)) ? 1 : 0;
            else
                next.grid[i][j] = (live_neighbors == 3) ? 1 : 0;
        }
    }
}

grid_t prior_grid(grid_t g) {
    
}
