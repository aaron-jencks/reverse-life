#include <stdio.h>
#include <stdlib.h>

#include "arraylist.h"
#include "error.h"

arraylist_t create_arraylist(size_t capacity) {
  arraylist_t result;
  result.count = 0;
  result.capacity = capacity;
  result.comparator = NULL;
  result.swap_updater = NULL;
  result.arr = (void**)malloc(sizeof(void*) * capacity);
  handle_memory_error(result.arr);
  return result;
}

void destroy_arraylist(arraylist_t arr) {
  free(arr.arr);
}

void destroy_arraylist_ptr(arraylist_t arr) {
  while(arr.count) free(arraylist_pop(&arr));
  free(arr.arr);
}

size_t arraylist_append(arraylist_t* arr, void* item) {
  if(arr->count == arr->capacity) {
    arr->capacity <<= 1;
    arr->arr = (void**)realloc(arr->arr, sizeof(void*) * arr->capacity);
    handle_memory_error(arr->arr);
  }
  arr->arr[arr->count] = item;
  return arr->count++;
}

void* arraylist_pop(arraylist_t* arr) {
  if(!arr->count) return (void*){0};
  return arr->arr[--arr->count];
}

void* arraylist_pop_front(arraylist_t* arr) {
  if(!arr->count) return (void*){0};
  void* result = arr->arr[0];
  for(size_t i = 0; i < arr->count; i++) {
    arr->arr[i-1] = arr->arr[i];
  }
  arr->count--;
  return result;
}

void* arraylist_remove(arraylist_t* arr, size_t index) {
  if(index >= arr->count) return (void*){0};
  void* result = arr->arr[index];
  for(size_t i = index + 1; i < arr->count; i++) {
    arr->arr[i - 1] = arr->arr[i];
  }
  arr->count--;
  return result;
}

bool arraylist_contains(arraylist_t arr, void* v) {
  if(!arr.comparator) handle_error(1, 
    "To use arraylist_contains, please define arr.comparator\n");
  for(size_t i = 0; i < arr.count; i++) {
    if(!arr.comparator(v, arr.arr[i])) return true;
  }
  return false;
}

void arraylist_swap(arraylist_t* arr, size_t a, size_t b) {
  void* temp = arr->arr[a];
  arr->arr[a] = arr->arr[b];
  arr->arr[b] = temp;
  if (arr->swap_updater) {
    arr->swap_updater(arr->arr[a], a);
    arr->swap_updater(temp, b);
  }
}

