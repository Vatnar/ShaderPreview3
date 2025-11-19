add_library(vui STATIC
    vui.c
    vui_renderer.c
    vui_font.c
)

find_package(X11 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)

target_link_libraries(vui PRIVATE X11::X11 OpenGL::GL Freetype::Freetype arena)
target_include_directories(vui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_include_directories(vui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../arena/include)

