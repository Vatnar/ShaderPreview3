#include "arena.h"
#include "debug_mem.h"
#include "hash_table.h"


int main(void)
{
   Arena arena;

    make_arena(5);
    make_arena_(5);

    return 0;
}
