#ifndef PRIORITY_H
#define PRIORITY_H

#include "arraylist.h"

void build_max_heap(arraylist_t* heap);
void heapsort(arraylist_t* heap);

void* heap_extract_max(arraylist_t* heap);

void heap_insert(arraylist_t* heap, void* item);

#endif