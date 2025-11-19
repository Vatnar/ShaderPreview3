#include "../vui/vui.h"
#include <stdio.h>
#include <GL/gl.h>
#include <stdbool.h>

int main() {
    int width = 1280;
    int height = 720;
    VuiWindow* window = vui_window_create(width, height, "VUI Shader Debugger Layout");
    if (!window) {
        fprintf(stderr, "Failed to create window\n");
        return 1;
    }
    
    // Load a nice font if available
    printf("Loading font...\n");
    VuiFont* font = vui_load_font(window, "/usr/share/fonts/TTF/HackNerdFontPropo-Regular.ttf", 18);
    if (font) {
        printf("Font loaded successfully\n");
        vui_set_font(window, font);
    } else {
        printf("Font failed to load\n");
    }

    while (!vui_window_should_close(window)) {
        vui_window_poll_events(window);
        
        // Clear background
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        vui_begin(window);
        
        int win_w, win_h;
        vui_get_window_size(window, &win_w, &win_h);
        
        // Root is a column by default
        
        // --- Menu Bar ---
        vui_begin_row(window, 25);
            // Background for menu bar?
            // We can draw a rect that fills the row.
            // But vui_rect advances cursor.
            // We need "layers" or "background" support.
            // For now, just draw buttons.
            if (vui_button(window, "\xEF\x81\xBC File")) printf("File\n");
            if (vui_button(window, "\xEF\x83\x87 Save")) printf("Save\n");
            if (vui_button(window, "\xEF\x80\x93 Settings")) printf("Settings\n");
        vui_end_row(window);
        
        // --- Toolbar ---
        vui_begin_row(window, 40);
            if (vui_button(window, "\xEF\x81\x8B")) printf("Start\n");
            if (vui_button(window, "\xEF\x81\x8C")) printf("Pause\n");
            if (vui_button(window, "\xEF\x81\x8D")) printf("Stop\n");
            vui_separator(window);
            if (vui_button(window, "\xEF\x81\x91")) printf("Step\n");
            if (vui_button(window, "\xEF\x83\xA2")) printf("Reset\n");
        vui_end_row(window);
        
        // --- Main Content (Row of Panels) ---
        // Height = remaining
        vui_begin_row(window, win_h - 70); // 25 + 40 + padding
            
            // Left Panel (Inspector)
            vui_begin_column(window, 250);
                vui_label(window, "INSPECTOR");
                vui_label(window, "----------------");
                
                static bool show_vars = true;
                if (vui_button(window, show_vars ? "[-] Variables" : "[+] Variables")) show_vars = !show_vars;
                
                if (show_vars) {
                    vui_begin_row(window, 0); // Indent trick? Or just a column with padding?
                    // No padding API yet.
                    // Just use a label with spaces for now.
                    vui_label(window, "  u_Time: 12.45");
                    vui_end_row(window);
                    vui_label(window, "  u_Res: 1280x720");
                }
            vui_end_column(window);
            
            // Center Area (Preview + Source)
            vui_begin_column(window, win_w - 250 - 300); // Calc remaining width
                
                // Preview (Square)
                // We want it to be square.
                // Available width is (win_w - 550).
                // Let's just draw a rect.
                int preview_size = win_w - 550 - 20; // minus padding
                if (preview_size > 400) preview_size = 400; // Cap size
                
                vui_rect(window, preview_size, preview_size, 0.0f, 0.0f, 0.0f, 1.0f);
                
                vui_label(window, "PREVIEW (1:1)");
                
                // Source Code
                vui_label(window, "\xEF\x84\xA0 SOURCE CODE");
                vui_label(window, "void main() {");
                vui_label(window, "    vec2 uv = gl_FragCoord.xy / u_Resolution.xy;");
                vui_label(window, "    gl_FragColor = vec4(uv, 0.0, 1.0);");
                vui_label(window, "}");
                
            vui_end_column(window);
            
            // Right Panel (Properties)
            vui_begin_column(window, 300);
                vui_label(window, "PROPERTIES");
                vui_label(window, "----------------");
                vui_button(window, "Compile Shader");
                vui_label(window, "Status: OK");
            vui_end_column(window);
            
        vui_end_row(window);
        
        vui_end(window);

        vui_window_swap_buffers(window);
    }

    vui_window_destroy(window);
    return 0;
}
