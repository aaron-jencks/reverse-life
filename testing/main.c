#include <stdbool.h>
#include <stdio.h>

#include "testing.h"

typedef bool (*test_case_t)();


#define TEST_COUNT 2
test_case_t test_cases[] = {
    test_priority_queue,
    test_heap_update_index
};



int main() {
    for(size_t i = 0; i < TEST_COUNT; i++) {
        if(!test_cases[i]()) {
            printf("Test %zd failed\n", i);
            return 1;
        }
        printf("Test %zd passed\n", i);
    }

    printf("All tests passed!\n");
    return 0;
}