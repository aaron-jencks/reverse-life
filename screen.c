#include <stdio.h>
#include <stdlib.h>

#include "screen.h"


coord_t get_terminal_size() {
    struct winsize w;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        perror("ioctl");
        exit(1);
    }

    coord_t result = {w.ws_col, w.ws_row};
    return result;
}