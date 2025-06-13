#ifndef CSP_H
#define CSP_H

#include <stdbool.h>

#include "grid.h"

typedef struct {
    grid_t result;
    bool found;
} parent_result_t;

parent_result_t find_parent_grid(grid_t g);

#endif