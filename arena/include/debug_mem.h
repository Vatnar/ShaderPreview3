#pragma once

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "hash_table.h"

/** NOTE: Only instruments malloc() and free().
 * If calloc() is used, wrap with: malloc(size) + memset(ptr, 0, size)
 *  realloc() is not supported; use malloc() + memcpy() + free() instead.
 **/
typedef struct
{
    void       *memory;
    size_t      size;
    const char *file;
    int         line;
} Allocation;

extern HashTable  hash_table;
extern HashTable *hash_table_ptr;
extern pthread_once_t once;

void debug_init_hash_table(void);

internal void *debug_malloc(const size_t size, const char *file, const int line)
{
    pthread_once(&once, debug_init_hash_table);

    assert(size > 0);
    if (size <= 0)
    {
        printf("Illegal size: %lu\n", size);
        exit(1);
    }

    void *ptr = malloc(size);
    if (ptr == NULL) return NULL;

    Allocation *alloc = (Allocation *) malloc(sizeof(Allocation));
    if (alloc == NULL)
    {
        printf("DEBUG_MALLOC: Failed to allocate Allocation struct\n");
        exit(1);
    }

    alloc->memory = ptr;
    alloc->file   = file;
    alloc->line   = line;
    alloc->size   = size;

    hash_table_insert(hash_table_ptr, ptr, alloc);
    return ptr;
}

internal void debug_free(void *ptr)
{
    if (ptr == NULL)
    {
        printf("DEBUG_FREE: Warning: Freeing NULL\n");
        return;
    }

    Allocation *alloc = (Allocation *) hash_table_lookup(hash_table_ptr, ptr);
    if (alloc == NULL)
    {
        printf("DEBUG_FREE: Warning: Freeing unknown pointer %p\n", ptr);
        exit(1);
    }

    free(alloc->memory);
    free(alloc);
    hash_table_delete(hash_table_ptr, ptr);
}

#define malloc(size) debug_malloc(size, __FILE__, __LINE__)
#define free(ptr) debug_free(ptr)

internal size_t debug_mem_check_allocations(void)
{
    size_t count = 0;

    for (size_t i = 0; i < hash_table.HASH_TABLE_SIZE; i++)
    {
        if (hash_table.entries[i].occupied == 1)
        {
            count++;
            Allocation *alloc = (Allocation *) hash_table.entries[i].value;

            fprintf(stderr,
                    "DEBUG_MEMORY: Memory allocated at %s:%d is not freed, "
                    "size: %lu\n",
                    alloc->file, alloc->line, alloc->size);
        }
    }

    if (count)
        printf("DEBUG_MEMORY: %lu instances of unfreed memory\n", count);
    else
        printf("DEBUG_MEMORY: No memory leaked\n");

    return count;
}

internal size_t debug_mem_remaining_frees(void)
{
    return hash_table_count(hash_table_ptr);
}

internal size_t debug_mem_end_summary()
{
    const size_t remaining_allocs = debug_mem_check_allocations();
    hash_table_cleanup(hash_table_ptr);
    return remaining_allocs;
}
