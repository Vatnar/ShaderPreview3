#include "arena.h"

#include <string.h>

#include "debug_mem.h"

Arena *arena_alloc_(U64 size, const char *file, int line)
{
    Arena *arena = malloc(sizeof(Arena));
    assert(arena != NULL);
    arena->size     = size;
    arena->base_pos = (U64) malloc(size);
    assert(arena->base_pos != (U64) NULL);
    arena->pos                  = 0;
    arena->allocation_site_file = malloc(strlen(file) + 1);
    strcpy(arena->allocation_site_file, file);
    arena->allocation_site_line = line;

    return arena;
}
void *arena_release(Arena *arena)
{
    free((void *) arena->base_pos);
    free(arena->allocation_site_file);
    free(arena);
    return NULL;
}

void *arena_push(Arena *arena, const U64 size, const B32 zero)
{
    U64 new_pos = arena->pos + size;
    if (arena->size < new_pos)
    {
        return NULL;
    }
    void *ret = (void *) (arena->base_pos + arena->pos);
    if (zero)
    {
        memset(ret, 0, size);
    }
    arena->pos = new_pos;
    return ret;
}
U64 arena_pos(const Arena *arena) { return arena->pos; }

void arena_pop_to(Arena *arena, U64 pos)
{
    assert((S64)pos - (S64)arena->base_pos >= 0);
    const U64 offset = pos - arena->base_pos;

    if (pos < arena->base_pos + arena->size && offset)
        arena->pos = offset;
}

void arena_clear(Arena *arena) { arena->pos = 0; }
void arena_pop(Arena *arena, const U64 amt)
{
    assert(arena->pos - amt >= 0);
    arena->pos -= amt;
}
