#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "../priority.h"
#include "../arraylist.h"

int int_comparator(const void* a, const void* b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

bool test_insert_extract() {
    arraylist_t heap = create_arraylist(10);
    heap.comparator = int_comparator;

    int values[] = { 5, 1, 8, 3, 7 };

    // Insert all elements
    for (size_t i = 0; i < sizeof(values)/sizeof(values[0]); i++) {
        heap_insert(&heap, &values[i]);
    }

    // Extract in decreasing order
    int last_value = 1000;
    while (heap.count > 0) {
        int* val = (int*)heap_extract_max(&heap);
        printf("Extracted: %d\n", *val);
        if (*val > last_value) {
            // Should be decreasing
            return false;
        }
        last_value = *val;
    }

    destroy_arraylist(heap);
    return true;
}

bool test_heapsort() {
    arraylist_t heap = create_arraylist(10);
    heap.comparator = int_comparator;

    int values[] = { 10, 2, 9, 4, 8, 6, 7 };

    // Add elements
    for (size_t i = 0; i < sizeof(values)/sizeof(values[0]); i++) {
        arraylist_append(&heap, &values[i]);
    }

    heapsort(&heap);

    // After heapsort, array should be in increasing order
    for (size_t i = 1; i < heap.count; i++) {
        int a = *(int*)heap.arr[i-1];
        int b = *(int*)heap.arr[i];
        printf("%d ", a);
        if (a > b) return false;
    }
    printf("\n");

    destroy_arraylist(heap);
    return true;
}

bool test_priority_queue() {
    printf("Running test_insert_extract...\n");
    if(!test_insert_extract()) return false;
    printf("Passed test_insert_extract!\n\n");

    printf("Running test_heapsort...\n");
    if(!test_heapsort()) return false;
    printf("Passed test_heapsort!\n\n");

    printf("All tests passed!\n");
    return true;
}


