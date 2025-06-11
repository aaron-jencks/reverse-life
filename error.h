#ifndef ERROR_H
#define ERROR_H

void handle_error(int status, char* fmt, ...);

void handle_memory_error(void* ptr);

#endif
