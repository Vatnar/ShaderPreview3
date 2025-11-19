#pragma once

typedef struct VuiWindow VuiWindow;
typedef struct VuiFont VuiFont;

VuiWindow* vui_window_create(int width, int height, const char* title);
void vui_window_destroy(VuiWindow* window);
int vui_window_should_close(VuiWindow* window);
void vui_window_poll_events(VuiWindow* window);
void vui_window_swap_buffers(VuiWindow* window);

int vui_key_down(VuiWindow* window, int key_code);
int vui_mouse_down(VuiWindow* window, int button);
void vui_mouse_pos(VuiWindow* window, int* x, int* y);

void vui_begin(VuiWindow* window);
void vui_end(VuiWindow* window);

void vui_begin_row(VuiWindow* window, float height);
void vui_end_row(VuiWindow* window);
void vui_begin_column(VuiWindow* window, float width);
void vui_end_column(VuiWindow* window);
void vui_separator(VuiWindow* window);

void vui_set_next_width(VuiWindow* window, int width);
void vui_same_line(VuiWindow* window);
void vui_set_cursor(VuiWindow* window, int x, int y);
void vui_get_cursor(VuiWindow* window, int* x, int* y);
void vui_get_window_size(VuiWindow* window, int* w, int* h);
void vui_rect(VuiWindow* window, int w, int h, float r, float g, float b, float a);

int vui_button(VuiWindow* window, const char* text);
void vui_label(VuiWindow* window, const char* text);

float vui_text_width(VuiWindow* window, const char* text);

VuiFont* vui_load_font(VuiWindow* window, const char* filename, int size);
void vui_set_font(VuiWindow* window, VuiFont* font);
