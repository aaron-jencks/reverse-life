#include "priority.h"
#include "arraylist.h"

#define parent(i) (((i) - 1) >> 1)
#define left(i) (((i) << 1) + 1)
#define right(i) (((i) << 1) + 2)

void max_heapify(arraylist_t* heap, size_t index, size_t n) {
  size_t l = left(index);
  size_t r = right(index);
  size_t largest = index;

  if(l < n && heap->comparator(heap->arr[l], heap->arr[index]) > 0)
    largest = l;
  if(r < n && heap->comparator(heap->arr[r], heap->arr[largest]) > 0)
    largest = r;

  if(largest != index) {
    arraylist_swap(heap, largest, index);
    max_heapify(heap, largest, n);
  }
}

void build_max_heap(arraylist_t* heap) {
  for(size_t i = heap->count ? (heap->count - 1) >> 1 : 0; i > 0; i--) {
    max_heapify(heap, i, heap->count);
  }
  max_heapify(heap, 0, heap->count);
}

void heapsort(arraylist_t* heap) {
  build_max_heap(heap);

  size_t last_leaf = heap->count - 1;
  while(last_leaf) {
    arraylist_swap(heap, 0, last_leaf);
    max_heapify(heap, 0, last_leaf--);
  }
}

void* heap_extract_max(arraylist_t* heap) {
  if(!heap->count) return 0;
  void* result = heap->arr[0];
  heap->arr[0] = heap->arr[--heap->count];
  max_heapify(heap, 0, heap->count);
  return result;
}

void heap_percolate(arraylist_t* heap, size_t index) {
    size_t p = parent(index);
    while(index && heap->comparator(heap->arr[p], heap->arr[index]) < 0) {
        p = parent(index);
        arraylist_swap(heap, p, index);
        index = p;
    }
}

void heap_insert(arraylist_t* heap, void* item) {
    arraylist_append(heap, item);
    heap_percolate(heap, heap->count - 1);
}

void heap_update_index(arraylist_t* heap, size_t index) {
    if(index > 0) {
        if (heap->comparator(heap->arr[parent(index)], heap->arr[index]) < 0) {
            // we need to percolate
            heap_percolate(heap, index);
            return;
        }
    }
    max_heapify(heap, index, heap->count);
}

#undef parent
#undef left
#undef right
