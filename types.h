#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    size_t x;
    size_t y;
} coord_t;

typedef struct {
    int64_t x;
    int64_t y;
} icoord_t;

#endif