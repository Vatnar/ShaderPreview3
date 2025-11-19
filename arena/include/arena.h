#pragma once
#include <stddef.h>
#include <string.h>
#include "typedefs.h"
#include "debug_mem.h"

#define Glue_(A, B) A##B
#define Glue(A, B) Glue_(A, B)

typedef struct
{
    U64   base_pos;
    U64   pos;
    U64   size;
    char *allocation_site_file;
    int   allocation_site_line;
} Arena;

internal inline Arena *arena_alloc_internal(const U64 size, const char *file, const int line)
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

#define arena_alloc(alloc_size) arena_alloc_internal(alloc_size, __FILE__, __LINE__)

void *arena_release(Arena *arena);

void *arena_push(Arena *arena, U64 size, B32 zero);
U64   arena_pos(const Arena *arena);
void  arena_pop_to(Arena *arena, U64 pos);

void arena_clear(Arena *arena);
void arena_pop(Arena *arena, U64 amt);
