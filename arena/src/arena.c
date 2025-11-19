#include "arena.h"
#include <string.h>
#include "debug_mem.h"

void *arena_release(Arena *arena)
{
    free((void *) arena->base_pos);
    free(arena->allocation_site_file);
    free(arena);
    return NULL;
}

internal B32 arena_can_fit(const Arena *arena, const U64 size)
{
    return arena->pos + size <= arena->size;
}

void *arena_push(Arena *arena, const U64 size, const B32 zero)
{
    if (!arena_can_fit(arena, size))
    {
        return NULL;
    }
    void *ret = (void *) (arena->base_pos + arena->pos);
    if (zero)
    {
        memset(ret, 0, size);
    }
    arena->pos += size;
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
