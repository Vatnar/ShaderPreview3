include (FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

add_executable(debug_mem_tests
        test_debug_mem.cpp
)

target_include_directories(debug_mem_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/arena/include
)

target_link_libraries(debug_mem_tests
        GTest::gtest_main
)

gtest_discover_tests(debug_mem_tests)
