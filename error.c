#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "error.h"

void handle_error(int status, char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  if(status) exit(status);
}

void handle_memory_error(void* ptr) {
  if(!ptr) handle_error(1, "out of memory\n");
}

