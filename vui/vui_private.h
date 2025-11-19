#ifndef VUI_PRIVATE_H
#define VUI_PRIVATE_H

#include <X11/Xlib.h>
#include <GL/glx.h>
#include <stdbool.h>

#include "vui.h"
#include "arena.h"
#include "typedefs.h"

#undef malloc
#undef free

typedef struct {
    float u0, v0, u1, v1; // Texture coordinates
    int width, height;    // Size in pixels
    int bearing_x, bearing_y;
    int advance;
} VuiGlyph;

struct VuiFont {
    unsigned int texture_id;
    int size;
    int height; // Max height
    
    VuiGlyph* glyphs;      // Array of loaded glyphs
    int glyph_count;
    int glyph_capacity;
    
    
    int ascii_map[128]; // Maps ASCII char to index in 'glyphs'
    
    struct {
        unsigned int codepoint;
        int index;
    } *extra_map;
    int extra_count;
    int extra_capacity;
};

struct VuiWindow {
    Display* display;
    Window window;
    GLXContext context;
    Atom wm_delete_window;
    
    Arena* arena;
    
    int width;
    int height;
    B32 should_close;
    
    int mouse_x, mouse_y;
    bool mouse_buttons[3]; // Left, Middle, Right
    bool mouse_buttons_prev[3];
    bool keys[256]; // Basic key tracking
    
    int cursor_x, cursor_y;
    int row_height;
    int next_width;
    bool same_line;
    
    VuiFont* active_font;
    VuiFont default_font;
    
    struct {
        int x, y, w, h;     // Rect of the current layout container
        int cursor_x, cursor_y; // Relative cursor within container
        int max_h;          // Max height of items in current row (for auto-height rows)
        int type;           // 0=Root, 1=Row, 2=Column
    } layout_stack[16];
    int layout_stack_depth;
};

void vui_renderer_init();
void vui_renderer_init_font(VuiWindow* window);
void vui_renderer_shutdown();
void vui_renderer_flush(VuiWindow* window);
void vui_render_quad(VuiWindow* win, float x, float y, float w, float h, float r, float g, float b, float a);
float vui_text_width(VuiWindow* win, const char* text);
void vui_render_text(VuiWindow* win, float x, float y, const char* text, float r, float g, float b, float a);

#endif // VUI_PRIVATE_H
