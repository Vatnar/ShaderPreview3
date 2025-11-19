#include "vui/vui.h"
#include <stdio.h>
#include <GL/gl.h>

int main(int argc, char **argv) {
    VuiWindow* window = vui_window_create(800, 600, "VUI Demo");
    if (!window) {
        fprintf(stderr, "Failed to create window\n");
        return 1;
    }
    VuiFont* font = NULL;
    font = vui_load_font(window, "/usr/share/fonts/TTF/HackNerdFontPropo-Regular.ttf", 24);
    if (font) vui_set_font(window, font);

    while (!vui_window_should_close(window)) {
        vui_window_poll_events(window);
        // Clear screen
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        vui_begin(window);
        
        if (vui_button(window, "Auto Size")) {
            printf("Auto Size Clicked!\n");
        }
        
        vui_same_line(window);
        
        if (vui_button(window, "Same Line")) {
            printf("Same Line Clicked!\n");
        }
        
        vui_set_next_width(window, 200);
        if (vui_button(window, "Fixed Width (200px)")) {
            printf("Fixed Width Clicked!\n");
        }
        
        vui_label(window, "This is a label");
        vui_end(window);

        vui_window_swap_buffers(window);
    }

    vui_window_destroy(window);
    return 0;
}
