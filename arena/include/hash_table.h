#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uintptr_t IPtr;

typedef struct
{
    void *key;
    void *value;
    int   occupied;
} HashTableEntry;

typedef struct
{
    size_t          HASH_TABLE_SIZE;
    HashTableEntry *entries;
} HashTable;

// circumvent defines in debug_mem.h
#define HT_MALLOC malloc
#define HT_FREE free

static size_t hash_pointer(const HashTable *hash_table, void *ptr)
{
    static const IPtr GOLDEN_RATIO = 0x9e3779b97f4a7c15UL;
    return ((IPtr) ptr * GOLDEN_RATIO) % hash_table->HASH_TABLE_SIZE;
}

static void hash_table_init(HashTable *hash_table)
{
    if (hash_table->entries == NULL)
    {
        hash_table->entries = (HashTableEntry *) HT_MALLOC(
                hash_table->HASH_TABLE_SIZE * sizeof(HashTableEntry));
        for (size_t i = 0; i < hash_table->HASH_TABLE_SIZE; i++)
        {
            hash_table->entries[i].key      = NULL;
            hash_table->entries[i].value    = NULL;
            hash_table->entries[i].occupied = 0;
        }
    }
}

static void hash_table_insert(HashTable *hash_table, void *key, void *value)
{
    size_t idx   = hash_pointer(hash_table, key);
    size_t start = idx;

    while (hash_table->entries[idx].occupied == 1)
    {
        idx = (idx + 1) % hash_table->HASH_TABLE_SIZE;
        if (idx == start)
        {
            printf("HASH_ERROR: HASH TABLE FULL\n");
            exit(1);
        }
    }

    HashTableEntry *entry = &hash_table->entries[idx];
    entry->key            = key;
    entry->value          = value;
    entry->occupied       = 1;
}

static void *hash_table_lookup(const HashTable *hash_table, void *key)
{
    size_t idx   = hash_pointer(hash_table, key);
    size_t start = idx;

    while (hash_table->entries[idx].occupied != 0)
    {
        if (hash_table->entries[idx].occupied == 1 &&
            hash_table->entries[idx].key == key)
        {
            return hash_table->entries[idx].value;
        }
        idx = (idx + 1) % hash_table->HASH_TABLE_SIZE;
        if (idx == start) break;
    }
    return NULL;
}

static void hash_table_delete(HashTable *hash_table, void *key)
{
    size_t idx   = hash_pointer(hash_table, key);
    size_t start = idx;

    while (hash_table->entries[idx].occupied != 0)
    {
        if (hash_table->entries[idx].occupied == 1 &&
            hash_table->entries[idx].key == key)
        {
            hash_table->entries[idx].key      = NULL;
            hash_table->entries[idx].value    = NULL;
            hash_table->entries[idx].occupied = -1;
            return;
        }
        idx = (idx + 1) % hash_table->HASH_TABLE_SIZE;
        if (idx == start) break;
    }
    printf("HASH_TABLE_ERROR: Key not found: %p\n", key);
    exit(1);
}

static size_t hash_table_count(const HashTable *hash_table)
{
    size_t count = 0;
    for (size_t i = 0; i < hash_table->HASH_TABLE_SIZE; i++)
        count += hash_table->entries[i].occupied == 1 ? 1 : 0;
    return count;
}


static void hash_table_cleanup(HashTable *hash_table)
{
    if (hash_table->entries != NULL)
    {
        for (size_t i = 0; i < hash_table->HASH_TABLE_SIZE; i++)
        {
            if (hash_table->entries[i].occupied == 1)
            {
                free(hash_table->entries[i].value);
            }
        }
        free(hash_table->entries);
        hash_table->entries = NULL;
    }
}
