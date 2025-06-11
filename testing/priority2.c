#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "../priority.h"
#include "../arraylist.h"

typedef struct {
    int priority;    // We will change this value
    size_t heap_index; // Required for heap_update_index
} TestItem;

int test_item_comparator(const void* a, const void* b) {
    const TestItem* ta = (const TestItem*)a;
    const TestItem* tb = (const TestItem*)b;
    return (ta->priority > tb->priority) - (ta->priority < tb->priority);
}

void test_item_update_heap_index(void* item, size_t new_index) {
    TestItem* t = (TestItem*)item;
    t->heap_index = new_index;
}

bool test_heap_update_index() {
    arraylist_t heap = create_arraylist(10);
    heap.comparator = test_item_comparator;
    heap.swap_updater = test_item_update_heap_index;

    // Create test items
    TestItem items[5];
    for (int i = 0; i < 5; i++) {
        items[i].priority = i + 1; // Priorities: 1,2,3,4,5
        items[i].heap_index = 0;   // Will be set during insert
        heap_insert(&heap, &items[i]);
    }

    // Manually change priority of one item
    // Let's pick items[2] (initial priority = 3)
    items[2].priority = 10; // Increased priority → should percolate up

    // Update heap
    heap_update_index(&heap, items[2].heap_index);

    // Extract all elements, check max-to-min order
    int last_priority = 1000;
    while (heap.count > 0) {
        TestItem* t = (TestItem*)heap_extract_max(&heap);
        printf("Extracted: %d\n", t->priority);
        if(t->priority > last_priority) return false;
        last_priority = t->priority;
    }

    destroy_arraylist(heap);
    printf("test_heap_update_index PASSED!\n");
    return true;
}
