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
        if (field.size.x < len) {
            // New max row length → must resize previous rows
            for (size_t i = 0; i < row_count; i++) {
                field.grid[i] = realloc(field.grid[i], len * sizeof(uint8_t));
                // Initialize new cells to 0
                for (size_t j = field.size.x; j < len; j++) {
                    field.grid[i][j] = 0;
                }
            }
            field.size.x = len;
        }

        field.grid[row_count] = calloc(field.size.x, sizeof(uint8_t));
        for (size_t i = 0; i < field.size.x; i++) {
            field.grid[row_count][i] = i < len ? (buffer[i] == '*' ? 1 : 0) : 0;
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
    return next;
}

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
uint8_t compress_neighbors(grid_t g, coord_t coordinate) {
    uint8_t result = 0;
    size_t bit = 7;
    for(int i = -1; i <= 1; i++) {
        for(int j = -1; j <= 1; j++) {
            if(!(i || j)) continue;
            if(i < 0 || i >= g.size.y || j < 0 || j >= g.size.x || !g.grid[i][j]) {
                bit--;
            } else {
                result += (1 << bit--);
            }
        }
    }
    return result;
}

icoord_t decode_neighbor_bit(uint8_t state, size_t bit, coord_t origin) {
    icoord_t result = {origin.x, origin.y};
    switch(bit) {
        case 7:
        result.y--;
        result.x--;
        return result;
        case 6:
        result.x--;
        return result;
        case 5:
        result.y++;
        result.x--;
        return result;
        case 4:
        result.y--;
        return result;
        case 3:
        result.y++;
        return result;
        case 2:
        result.y--;
        result.x++;
        return result;
        case 1:
        result.x++;
        return result;
        case 0:
        result.y++;
        result.x++;
        return result;
    }
}

/**
 * Determines the next state of a cell given the neighbors and it's current state
 */
bool get_state(uint8_t neighbors, bool state) {
    size_t neighbor_count = 0;
    for(size_t i = 0; i++ < 8;) {
        if(neighbors & 0x01) neighbor_count++;
        neighbors >>= 1;
    }
    return (state && (neighbor_count == 2 || neighbor_count == 3)) || (!state && neighbor_count == 3);
}