// test_debug_mem.c
#include "color/include/color.h"
#include "debug_mem.h"

int main(void) {
    void *p1 = malloc(100);
    void *p2 = malloc(200);
    // p2 intentionally leaked

    // debug_mem_end_summary();
    ERROR2("ValgrindMemCheck", "Failed to start.");
    return 0;
}

