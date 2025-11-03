#include "debug_mem.h"

HashTable hash_table;
HashTable *hash_table_ptr = &hash_table;
pthread_once_t once = PTHREAD_ONCE_INIT;

void           debug_init_hash_table(void)
{
    hash_table.entries         = NULL;
    hash_table.HASH_TABLE_SIZE = 1024;
    hash_table_init(hash_table_ptr);
}
