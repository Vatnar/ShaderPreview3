#include "arena.h"
#include "color.h"
#include "debug_mem.h"


int main(void)
{
    ERROR2("main", "um");
    ERROR2("debug_mem", "Something is wrong");
    ERROR2("debug_mem", "Failed to do this thing");
    ERROR2("debug_mem", "Subject[0] is not null");
    ERROR2("debug_mem", "Core dumped");
    return 0;
}
