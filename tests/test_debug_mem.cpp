#include <gtest/gtest.h>

extern "C"{
#include "debug_mem.h"
}

TEST(DebugMemTest, XORObfuscatedPointer) {
    size_t base = debug_mem_remaining_frees();
    void *raw = malloc(1024);
    EXPECT_EQ(debug_mem_remaining_frees(), base + 1);
}

TEST(DebugMemTest, SplitPointerAcrossGlobals) {
    size_t base = debug_mem_remaining_frees();
    EXPECT_EQ(debug_mem_remaining_frees(), base + 1);
}

TEST(DebugMemTest, TypeErasedInteger) {
    size_t base = debug_mem_remaining_frees();
}

TEST(DebugMemTest, PartiallyFreedContainer) {
    size_t base = debug_mem_remaining_frees();
    EXPECT_EQ(debug_mem_remaining_frees(), base + 2);  // Both allocations tracked
}
