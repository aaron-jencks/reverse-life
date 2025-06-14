#ifndef SCREEN_H
#define SCREEN_H

#include <sys/ioctl.h>
#include <unistd.h>

#include "types.h"


#define clear_screen() printf("\033[2J")

#define cursor_move(c) printf("\033[%zd;%zdH", (c.y+1), (c.x+1))

coord_t get_terminal_size();

#endif