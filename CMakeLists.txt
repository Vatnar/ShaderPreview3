cmake_minimum_required(VERSION 4.0)
project(ShaderPreview3 C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(arena)
add_subdirectory(color)

add_executable(scratch scratch.c)
add_executable(valgrind valgrind.c)
target_link_libraries(scratch PRIVATE arena)
target_link_libraries(valgrind PRIVATE arena)
target_link_libraries(scratch PRIVATE color)
add_subdirectory(tests)