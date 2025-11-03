#include "arena.h"
#include "debug_mem.h"

int main(void)
{
    Arena *int_arena = arena_alloc(48 * sizeof(int));
    int *x = arena_push(int_arena, sizeof(int), 1);
    int *y = arena_push(int_arena, sizeof(int), 1);
    int *z = arena_push(int_arena, sizeof(int), 1);
    *x = 5;
    *y = 10;
    *z = 20;

    arena_pop(int_arena, sizeof(int));
    int *z2 = arena_push(int_arena, sizeof(int), 1);
    *z2 = -2222;


    arena_clear(int_arena);
    arena_release(int_arena);
    debug_mem_end_summary();

    return 0;
}
