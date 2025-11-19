#include "vui_private.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glx.h>

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
static glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;

static bool isExtensionSupported(const char *extList, const char *extension) {
    const char *start;
    const char *where, *terminator;
    
    where = strchr(extension, ' ');
    if (where || *extension == '\0')
        return false;

    for (start = extList; ; ) {
        where = strstr(start, extension);
        if (!where)
            break;
        terminator = where + strlen(extension);
        if (where == start || *(where - 1) == ' ')
            if (*terminator == ' ' || *terminator == '\0')
                return true;
        start = terminator;
    }
    return false;
}


static void push_layout(VuiWindow* win, int x, int y, int w, int h, int type) {
    if (win->layout_stack_depth >= 15) return;
    win->layout_stack_depth++;
    win->layout_stack[win->layout_stack_depth].x = x;
    win->layout_stack[win->layout_stack_depth].y = y;
    win->layout_stack[win->layout_stack_depth].w = w;
    win->layout_stack[win->layout_stack_depth].h = h;
    win->layout_stack[win->layout_stack_depth].cursor_x = 0;
    win->layout_stack[win->layout_stack_depth].cursor_y = 0;
    win->layout_stack[win->layout_stack_depth].max_h = 0;
    win->layout_stack[win->layout_stack_depth].type = type;
}

static void pop_layout(VuiWindow* win) {
    if (win->layout_stack_depth > 0) {
        int w = win->layout_stack[win->layout_stack_depth].w;
        int h = win->layout_stack[win->layout_stack_depth].h;
        
        int type = win->layout_stack[win->layout_stack_depth].type;
        int actual_w = win->layout_stack[win->layout_stack_depth].cursor_x;
        int actual_h = win->layout_stack[win->layout_stack_depth].cursor_y;
        if (type == 1) actual_h = win->layout_stack[win->layout_stack_depth].max_h;
        
        win->layout_stack_depth--;
        
        if (win->layout_stack[win->layout_stack_depth].type == 1) { // Parent is Row
             win->layout_stack[win->layout_stack_depth].cursor_x += (w > 0 ? w : actual_w) + 5;
             if ((h > 0 ? h : actual_h) > win->layout_stack[win->layout_stack_depth].max_h)
                 win->layout_stack[win->layout_stack_depth].max_h = (h > 0 ? h : actual_h);
        } else { // Parent is Column/Root
             win->layout_stack[win->layout_stack_depth].cursor_y += (h > 0 ? h : actual_h) + 5;
        }
    }
}

static void layout_item(VuiWindow* win, int w, int h, int* x, int* y) {
    int idx = win->layout_stack_depth;
    *x = win->layout_stack[idx].x + win->layout_stack[idx].cursor_x;
    *y = win->layout_stack[idx].y + win->layout_stack[idx].cursor_y;
    
    if (win->layout_stack[idx].type == 1) { // Row
        win->layout_stack[idx].cursor_x += w + 5;
        if (h > win->layout_stack[idx].max_h) win->layout_stack[idx].max_h = h;
    } else { // Column/Root
        win->layout_stack[idx].cursor_y += h + 5;
    }
}

VuiWindow* 
vui_window_create(int width, int height, const char* title)
{
    VuiWindow* win = malloc(sizeof(VuiWindow));
    memset(win, 0, sizeof(VuiWindow));
    
    win->arena = arena_alloc(MB(16));
    win->width = width;
    win->height = height;
    
    win->display = XOpenDisplay(NULL);
    if(!win->display) {
        fprintf(stderr, "Failed to open X display\n");
        arena_release(win->arena);
        free(win);
        return NULL;
    }
    
    int glx_major, glx_minor;
    glXQueryVersion(win->display, &glx_major, &glx_minor);
    if(glx_major < 1 || (glx_major == 1 && glx_minor < 3)) {
        fprintf(stderr, "GLX 1.3 or greater is required\n");
        XCloseDisplay(win->display);
        arena_release(win->arena); free(win);
        return NULL;
    }
    
    int fb_attribs[] = {
        GLX_X_RENDERABLE, True,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        GLX_STENCIL_SIZE, 8,
        GLX_DOUBLEBUFFER, True,
        None
    };
    
    int fb_count;
    GLXFBConfig* fbc = glXChooseFBConfig(win->display, DefaultScreen(win->display), fb_attribs, &fb_count);
    if(!fbc) {
        fprintf(stderr, "Failed to retrieve framebuffer config\n");
        XCloseDisplay(win->display);
        arena_release(win->arena); free(win);
        return NULL;
    }
    
    GLXFBConfig best_fbc = fbc[0];
    XVisualInfo* vi = glXGetVisualFromFBConfig(win->display, best_fbc);
    
    XSetWindowAttributes swa;
    swa.colormap = XCreateColormap(win->display, RootWindow(win->display, vi->screen), vi->visual, AllocNone);
    swa.border_pixel = 0;
    swa.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask | 
                     ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ExposureMask;
    
    win->window = XCreateWindow(win->display, RootWindow(win->display, vi->screen), 
                                0, 0, width, height, 0, vi->depth, InputOutput, 
                                vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa);
                                
    if(!win->window) {
        fprintf(stderr, "Failed to create window\n");
        XFree(fbc);
        XFree(vi);
        XCloseDisplay(win->display);
        arena_release(win->arena); free(win);
        return NULL;
    }
    
    XStoreName(win->display, win->window, title);
    
    win->wm_delete_window = XInternAtom(win->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(win->display, win->window, &win->wm_delete_window, 1);
    
    XMapWindow(win->display, win->window);
    
    const char *glx_exts = glXQueryExtensionsString(win->display, DefaultScreen(win->display));
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
        glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");
    
    GLXContext ctx = 0;
    if(!isExtensionSupported(glx_exts, "GLX_ARB_create_context") || !glXCreateContextAttribsARB) {
        fprintf(stderr, "GLX_ARB_create_context not supported\n");
        ctx = glXCreateNewContext(win->display, best_fbc, GLX_RGBA_TYPE, 0, True);
    }
    else {
        int context_attribs[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
            GLX_CONTEXT_MINOR_VERSION_ARB, 6,
            GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
            None
        };
        ctx = glXCreateContextAttribsARB(win->display, best_fbc, 0, True, context_attribs);
    }
    
    XFree(vi);
    
    if(!ctx) {
        fprintf(stderr, "Failed to create OpenGL context\n");
        XDestroyWindow(win->display, win->window);
        XCloseDisplay(win->display);
        arena_release(win->arena); free(win);
        return NULL;
    }
    
    win->context = ctx;
    glXMakeCurrent(win->display, win->window, ctx);
    
    vui_renderer_init();
    vui_renderer_init_font(win);
    
    return win;
}

void 
vui_window_destroy(VuiWindow* window)
{
    if(!window) return;
    
    if(window->context) {
        glXMakeCurrent(window->display, None, NULL);
        glXDestroyContext(window->display, window->context);
    }
    
    if(window->window) {
        XDestroyWindow(window->display, window->window);
    }
    
    if(window->display) {
        XCloseDisplay(window->display);
    }
    
    arena_release(window->arena);
    free(window);
}

int 
vui_window_should_close(VuiWindow* window)
{
    return window->should_close;
}

void 
vui_window_poll_events(VuiWindow* window)
{
    while(XPending(window->display) > 0) {
        XEvent event;
        XNextEvent(window->display, &event);
        
        if(event.type == ClientMessage) {
            if((Atom)event.xclient.data.l[0] == window->wm_delete_window) {
                window->should_close = 1;
            }
        }
        else if(event.type == ButtonPress) {
            if(event.xbutton.button >= 1 && event.xbutton.button <= 3) {
                window->mouse_buttons[event.xbutton.button - 1] = 1;
            }
        }
        else if(event.type == ButtonRelease) {
            if(event.xbutton.button >= 1 && event.xbutton.button <= 3) {
                window->mouse_buttons[event.xbutton.button - 1] = 0;
            }
        }
        else if(event.type == MotionNotify) {
            window->mouse_x = event.xmotion.x;
            window->mouse_y = event.xmotion.y;
        }
        else if(event.type == ConfigureNotify) {
            window->width = event.xconfigure.width;
            window->height = event.xconfigure.height;
            glViewport(0, 0, window->width, window->height);
        }
    }
}

void 
vui_window_swap_buffers(VuiWindow* window)
{
    glXSwapBuffers(window->display, window->window);
}

void 
vui_begin(VuiWindow* window)
{
    window->layout_stack_depth = 0;
    window->layout_stack[0].x = 0;
    window->layout_stack[0].y = 0;
    window->layout_stack[0].w = window->width;
    window->layout_stack[0].h = window->height;
    window->layout_stack[0].cursor_x = 0;
    window->layout_stack[0].cursor_y = 0;
    window->layout_stack[0].type = 2;
    
    window->cursor_x = 0;
    window->cursor_y = 0;
    window->row_height = 0;
    window->next_width = 0;
    window->same_line = 0;
}

void 
vui_end(VuiWindow* window)
{
    vui_renderer_flush(window);
    
    for(int i = 0; i < 3; i++) {
        window->mouse_buttons_prev[i] = window->mouse_buttons[i];
    }
}

void vui_set_next_width(VuiWindow* window, int width) { window->next_width = width; }
void vui_same_line(VuiWindow* window) { window->same_line = 1; }
void vui_set_cursor(VuiWindow* window, int x, int y) { window->cursor_x = x; window->cursor_y = y; }
void vui_get_cursor(VuiWindow* window, int* x, int* y) { if(x) *x = window->cursor_x; if(y) *y = window->cursor_y; }
void vui_get_window_size(VuiWindow* window, int* w, int* h) { if(w) *w = window->width; if(h) *h = window->height; }


int 
vui_button(VuiWindow* window, const char* text)
{
    int padding = 10;
    float text_w = vui_text_width(window, text);
    float text_h = (window->active_font ? window->active_font->height : 14);
    
    int w = (int)text_w + padding * 2;
    int h = (int)text_h + padding * 2;
    
    if(window->next_width > 0) {
        w = window->next_width;
        window->next_width = 0;
    }
    
    int x, y;
    layout_item(window, w, h, &x, &y);
    
    int hovered = (window->mouse_x >= x && window->mouse_x <= x + w &&
                   window->mouse_y >= y && window->mouse_y <= y + h);
    int clicked = hovered && window->mouse_buttons[0] && !window->mouse_buttons_prev[0];
    
    float r = 0.3f, g = 0.3f, b = 0.3f;
    if(hovered) { r = 0.4f; g = 0.4f; b = 0.4f; }
    if(window->mouse_buttons[0] && hovered) { r = 0.2f; g = 0.2f; b = 0.2f; }
    
    vui_render_quad(window, x, y, w, h, r, g, b, 1.0f);
    vui_render_text(window, x + padding, y + padding, text, 1.0f, 1.0f, 1.0f, 1.0f);
    
    return clicked;
}

void 
vui_label(VuiWindow* window, const char* text)
{
    int w = vui_text_width(window, text);
    int h = 20;
    int x, y;
    layout_item(window, w, h, &x, &y);
    vui_render_text(window, x, y, text, 1.0f, 1.0f, 1.0f, 1.0f);
}


void 
vui_begin_row(VuiWindow* window, float height)
{
    int parent_idx = window->layout_stack_depth;
    int x = window->layout_stack[parent_idx].x + window->layout_stack[parent_idx].cursor_x;
    int y = window->layout_stack[parent_idx].y + window->layout_stack[parent_idx].cursor_y;
    int w = 0;
    push_layout(window, x, y, w, (int)height, 1);
}

void vui_end_row(VuiWindow* window) { pop_layout(window); }

void 
vui_begin_column(VuiWindow* window, float width)
{
    int parent_idx = window->layout_stack_depth;
    int x = window->layout_stack[parent_idx].x + window->layout_stack[parent_idx].cursor_x;
    int y = window->layout_stack[parent_idx].y + window->layout_stack[parent_idx].cursor_y;
    int h = 0;
    push_layout(window, x, y, (int)width, h, 2);
}

void vui_end_column(VuiWindow* window) { pop_layout(window); }

void 
vui_separator(VuiWindow* window)
{
    int idx = window->layout_stack_depth;
    if(window->layout_stack[idx].type == 1) {
        window->layout_stack[idx].cursor_x += 5;
    } else {
        window->layout_stack[idx].cursor_y += 5;
    }
}

void 
vui_rect(VuiWindow* window, int w, int h, float r, float g, float b, float a)
{
    int x, y;
    if(window->next_width > 0) {
        w = window->next_width;
        window->next_width = 0;
    }
    layout_item(window, w, h, &x, &y);
    vui_render_quad(window, x, y, w, h, r, g, b, a);
}

int vui_key_down(VuiWindow* window, int key_code) { return 0; }
int vui_mouse_down(VuiWindow* window, int button) { 
    if(button >= 0 && button < 3) return window->mouse_buttons[button];
    return 0;
}
void vui_mouse_pos(VuiWindow* window, int* x, int* y) {
    if(x) *x = window->mouse_x;
    if(y) *y = window->mouse_y;
}
