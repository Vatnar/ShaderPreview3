#pragma once
#include <stddef.h>
#include "typedefs.h"
#include "debug_mem.h"


#define Glue_(A, B) A##B
#define Glue(A, B) Glue_(A, B)
#define StaticAssert(C, ID) global U8 Glue(ID, __LINE__)[(C) ? 1 : -1]


typedef struct
{
    U64   base_pos;
    U64   pos;
    U64   size;
    char *allocation_site_file;
    int   allocation_site_line;
} Arena;

global U64 arena_default_reserve_size = MB(64);
global U64 arena_default_commit_size  = KB(64);

Arena *arena_alloc_(U64 size, const char *file, int line);
#define arena_alloc(alloc_size) arena_alloc_(alloc_size, __FILE__, __LINE__)
void *arena_release(Arena *arena);

void *arena_push(Arena *arena, U64 size, B32 zero);
U64   arena_pos(const Arena *arena);
void  arena_pop_to(Arena *arena, U64 pos);

void arena_clear(Arena *arena);
void arena_pop(Arena *arena, U64 amt);
