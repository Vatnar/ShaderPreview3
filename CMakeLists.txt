cmake_minimum_required(VERSION 4.0)
project(ShaderPreview3 C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(arena)
add_subdirectory(color)
add_subdirectory(vui)

find_package(PkgConfig REQUIRED)


add_executable(scratch src/main.c)
target_link_libraries(scratch PRIVATE vui)

add_executable(demolayout src/demolayout.c)
target_link_libraries(demolayout PRIVATE vui)
add_executable(valgrind valgrind.c)


target_link_libraries(valgrind PRIVATE arena)
add_subdirectory(tests)