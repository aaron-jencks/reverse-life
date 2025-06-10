#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "screen.h"


typedef struct {
    bool** grid;
    size_t rows;
    size_t columns;
    uint8_t birth_count;
    uint8_t sustain_count;
    uint8_t starve_count;
} field_t;

field_t load_field(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("fopen");
        exit(1);
    }

    field_t field;
    fscanf(f, "%hhu %hhu %hhu\n", &field.birth_count, &field.sustain_count, &field.starve_count);

    char buffer[1024];
    size_t capacity = 16;
    size_t row_count = 0;
    field.grid = malloc(capacity * sizeof(bool*));
    field.columns = 0;

    while (fgets(buffer, 1024, f)) {
        size_t len = strcspn(buffer, "\n");
        if (row_count >= capacity) {
            capacity *= 2;
            field.grid = realloc(field.grid, capacity * sizeof(bool*));
        }
        if (field.columns < len)
            field.columns = len;

        field.grid[row_count] = calloc(len, sizeof(bool));
        for (size_t i = 0; i < len; i++) {
            field.grid[row_count][i] = buffer[i] == '*';
        }
        row_count++;
    }

    field.rows = row_count;
    fclose(f);
    return field;
}

bool** allocate_grid(size_t rows, size_t cols) {
    bool** grid = malloc(rows * sizeof(bool*));
    for (size_t i = 0; i < rows; i++) {
        grid[i] = calloc(cols, sizeof(bool));
    }
    return grid;
}

void free_grid(bool** grid, size_t rows) {
    for (size_t i = 0; i < rows; i++)
        free(grid[i]);
    free(grid);
}

void copy_into_center(bool** dest, size_t drows, size_t dcols,
                      bool** src, size_t srows, size_t scols) {
    size_t row_offset = (drows > srows) ? (drows - srows) / 2 : 0;
    size_t col_offset = (dcols > scols) ? (dcols - scols) / 2 : 0;

    for (size_t i = 0; i < srows && (row_offset + i) < drows; i++) {
        for (size_t j = 0; j < scols && (col_offset + j) < dcols; j++) {
            dest[row_offset + i][col_offset + j] = src[i][j];
        }
    }
}


bool** update_field(field_t* field) {
    bool** next = allocate_grid(field->rows, field->columns);

    for (size_t i = 0; i < field->rows; i++) {
        for (size_t j = 0; j < field->columns; j++) {
            int live_neighbors = 0;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) continue;
                    size_t ni = i + dx;
                    size_t nj = j + dy;
                    if (ni >= 0 && ni < field->rows && nj >= 0 && nj < field->columns)
                        live_neighbors += field->grid[ni][nj];
                }
            }

            if (field->grid[i][j])
                next[i][j] = (live_neighbors < field->starve_count) && (live_neighbors >= field->sustain_count);
            else
                next[i][j] = (live_neighbors == field->birth_count);
        }
    }

    bool** previous = field->grid;
    field->grid = next;
    return previous;
}

void draw_field(field_t* current, field_t* previous, coord_t term) {
    coord_t cc;
    for (size_t i = 0; i < current->rows && i < term.y; i++) {
        for (size_t j = 0; j < current->columns && j < term.x; j++) {
            if (current->grid[i][j] != previous->grid[i][j]) {
                cc.x = j;
                cc.y = i;
                cursor_move(cc);
                putchar(current->grid[i][j] ? '*' : ' ');
                previous->grid[i][j] = current->grid[i][j];
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
    field_t input = load_field(argv[1]);

    // 3. Allocate full-screen field
    field_t current = {
        .rows = term.y,
        .columns = term.x,
        .birth_count = input.birth_count,
        .sustain_count = input.sustain_count,
        .starve_count = input.starve_count,
        .grid = allocate_grid(term.y, term.x),
    };

    copy_into_center(current.grid, current.rows, current.columns,
                     input.grid, input.rows, input.columns);
    free_grid(input.grid, input.rows);

    // 4. Allocate previous grid
    field_t previous = {
        .rows = term.y,
        .columns = term.x,
        .grid = allocate_grid(term.y, term.x),
    };

    clear_screen();

    // 5. Simulation loop
    while (true) {
        draw_field(&current, &previous, term);
        usleep(100000);
        update_field(&current);
    }

    return 0;
}


