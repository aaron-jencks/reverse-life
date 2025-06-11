#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <stdbool.h>
#include <stddef.h>

typedef int (*comparator_t)(const void*, const void*);
typedef void (*swap_updater_t)(void*, size_t);

typedef struct {
  void** arr;
  size_t capacity;
  size_t count;
  comparator_t comparator;
  swap_updater_t swap_updater;
} arraylist_t;

arraylist_t create_arraylist(size_t capacity);

void destroy_arraylist(arraylist_t arr);
void destroy_arraylist_ptr(arraylist_t arr);

size_t arraylist_append(arraylist_t* arr, void* item);

void* arraylist_pop(arraylist_t* arr);
void* arraylist_pop_front(arraylist_t* arr);

void* arraylist_remove(arraylist_t* arr, size_t index);

bool arraylist_contains(arraylist_t arr, void* v);

void arraylist_swap(arraylist_t* arr, size_t a, size_t b);

#endif
