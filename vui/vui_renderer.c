#include "vui_private.h"
#include "vui_font.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glext.h>

static PFNGLCREATESHADERPROC glCreateShader = NULL;
static PFNGLSHADERSOURCEPROC glShaderSource = NULL;
static PFNGLCOMPILESHADERPROC glCompileShader = NULL;
static PFNGLGETSHADERIVPROC glGetShaderiv = NULL;
static PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = NULL;
static PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
static PFNGLATTACHSHADERPROC glAttachShader = NULL;
static PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
static PFNGLGETPROGRAMIVPROC glGetProgramiv = NULL;
static PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NULL;
static PFNGLDELETESHADERPROC glDeleteShader = NULL;
static PFNGLUSEPROGRAMPROC glUseProgram = NULL;
static PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = NULL;
static PFNGLBINDVERTEXARRAYPROC glBindVertexArray = NULL;
static PFNGLGENBUFFERSPROC glGenBuffers = NULL;
static PFNGLBINDBUFFERPROC glBindBuffer = NULL;
static PFNGLBUFFERDATAPROC glBufferData = NULL;
static PFNGLBUFFERSUBDATAPROC glBufferSubData = NULL;
static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = NULL;
static PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = NULL;
static PFNGLUNIFORM2FPROC glUniform2f = NULL;
static PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
static PFNGLUNIFORM1IPROC glUniform1i = NULL;



static void load_gl_funcs() {
    glCreateShader = (PFNGLCREATESHADERPROC)glXGetProcAddress((const GLubyte*)"glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)glXGetProcAddress((const GLubyte*)"glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)glXGetProcAddress((const GLubyte*)"glCompileShader");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)glXGetProcAddress((const GLubyte*)"glGetShaderiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)glXGetProcAddress((const GLubyte*)"glGetShaderInfoLog");
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)glXGetProcAddress((const GLubyte*)"glCreateProgram");
    glAttachShader = (PFNGLATTACHSHADERPROC)glXGetProcAddress((const GLubyte*)"glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)glXGetProcAddress((const GLubyte*)"glLinkProgram");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)glXGetProcAddress((const GLubyte*)"glGetProgramiv");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)glXGetProcAddress((const GLubyte*)"glGetProgramInfoLog");
    glDeleteShader = (PFNGLDELETESHADERPROC)glXGetProcAddress((const GLubyte*)"glDeleteShader");
    glUseProgram = (PFNGLUSEPROGRAMPROC)glXGetProcAddress((const GLubyte*)"glUseProgram");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glXGetProcAddress((const GLubyte*)"glGenVertexArrays");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glXGetProcAddress((const GLubyte*)"glBindVertexArray");
    glGenBuffers = (PFNGLGENBUFFERSPROC)glXGetProcAddress((const GLubyte*)"glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)glXGetProcAddress((const GLubyte*)"glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)glXGetProcAddress((const GLubyte*)"glBufferData");
    glBufferSubData = (PFNGLBUFFERSUBDATAPROC)glXGetProcAddress((const GLubyte*)"glBufferSubData");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glXGetProcAddress((const GLubyte*)"glEnableVertexAttribArray");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)glXGetProcAddress((const GLubyte*)"glVertexAttribPointer");
    glUniform2f = (PFNGLUNIFORM2FPROC)glXGetProcAddress((const GLubyte*)"glUniform2f");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)glXGetProcAddress((const GLubyte*)"glGetUniformLocation");
    glUniform1i = (PFNGLUNIFORM1IPROC)glXGetProcAddress((const GLubyte*)"glUniform1i");
}

static const char* vs_source = 
    "#version 460 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "layout (location = 1) in vec4 aColor;\n"
    "layout (location = 2) in vec2 aTexCoord;\n"
    "out vec4 vColor;\n"
    "out vec2 vTexCoord;\n"
    "uniform vec2 uScreenSize;\n"
    "void main() {\n"
    "    vec2 ndc = (aPos / uScreenSize) * 2.0 - 1.0;\n"
    "    ndc.y = -ndc.y;\n"
    "    gl_Position = vec4(ndc, 0.0, 1.0);\n"
    "    vColor = aColor;\n"
    "    vTexCoord = aTexCoord;\n"
    "}\n";

static const char* fs_source = 
    "#version 460 core\n"
    "in vec4 vColor;\n"
    "in vec2 vTexCoord;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D uTexture;\n"
    "uniform bool uUseTexture;\n"
    "void main() {\n"
    "    if (uUseTexture) {\n"
    "        float alpha = texture(uTexture, vTexCoord).r;\n"
    "        FragColor = vec4(vColor.rgb, vColor.a * alpha);\n"
    "    } else {\n"
    "        FragColor = vColor;\n"
    "    }\n"
    "}\n";

#define MAX_VERTICES 10000

typedef struct {
    float x, y;
    float r, g, b, a;
    float u, v;
} Vertex;

static GLuint shader_program;
static GLuint vao, vbo;
static GLuint font_texture;
static Vertex vertices[MAX_VERTICES];
static int vertex_count = 0;
static GLint u_screen_size_loc;
static GLint u_use_texture_loc;
static GLint u_texture_loc;

static GLuint compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "Shader compilation failed: %s\n", infoLog);
    }
    return shader;
}

void vui_renderer_init() {
    load_gl_funcs();
    
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vs_source);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fs_source);
    
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vs);
    glAttachShader(shader_program, fs);
    glLinkProgram(shader_program);
    
    GLint success;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
        fprintf(stderr, "Shader linking failed: %s\n", infoLog);
    }
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    u_screen_size_loc = glGetUniformLocation(shader_program, "uScreenSize");
    u_use_texture_loc = glGetUniformLocation(shader_program, "uUseTexture");
    u_texture_loc = glGetUniformLocation(shader_program, "uTexture");
    
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), NULL, GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glGenTextures(1, &font_texture);
    glBindTexture(GL_TEXTURE_2D, font_texture);
    
    int tex_w = 128;
    int tex_h = 128;
    unsigned char* tex_data = (unsigned char*)calloc(tex_w * tex_h, 1);
    
    
    for (int i = 0; i < 96; i++) {
        int cx = (i % 16) * 6; // 6 pixels wide (5 + 1 spacing)
        int cy = (i / 16) * 8; // 8 pixels high (7 + 1 spacing)
        
        for (int col = 0; col < 5; col++) {
            unsigned char col_data = font5x7[i][col];
            for (int row = 0; row < 7; row++) {
                if ((col_data >> row) & 1) {
                    tex_data[(cy + row) * tex_w + (cx + col)] = 255;
                }
            }
        }
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, tex_w, tex_h, 0, GL_RED, GL_UNSIGNED_BYTE, tex_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    free(tex_data);
}

void vui_renderer_init_font(VuiWindow* window) {
    window->default_font.texture_id = font_texture;
    window->default_font.size = 7;
    window->default_font.height = 7;
    
    window->default_font.size = 7;
    window->default_font.height = 7;
    
    window->default_font.glyph_capacity = 128;
    window->default_font.glyphs = (VuiGlyph*)calloc(128, sizeof(VuiGlyph));
    
    for (int i=0; i<128; i++) window->default_font.ascii_map[i] = i;
    
    for (int i = 0; i < 96; i++) {
        int cx = (i % 16) * 6;
        int cy = (i / 16) * 8;
        int char_idx = i + 32;
        
        window->default_font.glyphs[char_idx].u0 = (float)cx / 128.0f;
        window->default_font.glyphs[char_idx].v0 = (float)cy / 128.0f;
        window->default_font.glyphs[char_idx].u1 = (float)(cx + 5) / 128.0f;
        window->default_font.glyphs[char_idx].v1 = (float)(cy + 7) / 128.0f;
        window->default_font.glyphs[char_idx].width = 5;
        window->default_font.glyphs[char_idx].height = 7;
        window->default_font.glyphs[char_idx].bearing_x = 0;
        window->default_font.glyphs[char_idx].bearing_y = 7; // Top is 7
        window->default_font.glyphs[char_idx].advance = 6;
    }
    
    window->active_font = &window->default_font;
}

void vui_renderer_flush(VuiWindow* window) {
    if (vertex_count == 0) return;
    
    glUseProgram(shader_program);
    glUniform2f(u_screen_size_loc, (float)window->width, (float)window->height);
    
    glActiveTexture(GL_TEXTURE0);
    if (window->active_font) {
        glBindTexture(GL_TEXTURE_2D, window->active_font->texture_id);
    } else {
        glBindTexture(GL_TEXTURE_2D, font_texture); // Fallback
    }
    glUniform1i(u_texture_loc, 0);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_count * sizeof(Vertex), vertices);
    
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    
    glBindVertexArray(0);
    glUseProgram(0);
    
    vertex_count = 0;
}

static bool current_use_texture = false;

void check_flush(VuiWindow* win, bool use_texture) {
    if (use_texture != current_use_texture) {
        vui_renderer_flush(win);
        current_use_texture = use_texture;
        
        glUseProgram(shader_program);
        glUniform1i(u_use_texture_loc, use_texture);
        glUseProgram(0);
    }
}

void vui_render_quad(VuiWindow* win, float x, float y, float w, float h, float r, float g, float b, float a) {
    check_flush(win, false);
    if (vertex_count + 6 > MAX_VERTICES) vui_renderer_flush(win);
    
    Vertex* v = &vertices[vertex_count];
    float u = 0, v_ = 0;
    v[0] = (Vertex){x, y, r, g, b, a, u, v_};
    v[1] = (Vertex){x, y + h, r, g, b, a, u, v_};
    v[2] = (Vertex){x + w, y, r, g, b, a, u, v_};
    v[3] = (Vertex){x + w, y, r, g, b, a, u, v_};
    v[4] = (Vertex){x, y + h, r, g, b, a, u, v_};
    v[5] = (Vertex){x + w, y + h, r, g, b, a, u, v_};
    vertex_count += 6;
}

void vui_render_char(VuiWindow* win, float x, float y, unsigned int codepoint, float r, float g, float b, float a) {
    check_flush(win, true);
    if (vertex_count + 6 > MAX_VERTICES) vui_renderer_flush(win);
    
    if (!win->active_font) return;
    VuiFont* font = win->active_font;
    float scale = (font == &win->default_font) ? 2.0f : 1.0f;
    
    int glyph_idx = -1;
    
    if (font == &win->default_font) {
        if (codepoint < 128) glyph_idx = codepoint;
    } else {
        if (codepoint < 128) {
            glyph_idx = font->ascii_map[codepoint];
        } else {
            for (int i = 0; i < font->extra_count; i++) {
                if (font->extra_map[i].codepoint == codepoint) {
                    glyph_idx = font->extra_map[i].index;
                    break;
                }
            }
        }
    }
    
    if (glyph_idx < 0) return; // Glyph not found

    VuiGlyph* g_info = &font->glyphs[glyph_idx];
    
    float xpos = x + g_info->bearing_x * scale;
    float ypos = y;
    float w = g_info->width * scale;
    float h = g_info->height * scale;
    
    if (font != &win->default_font) {
         ypos = y + (font->size - g_info->bearing_y);
    }

    Vertex* v = &vertices[vertex_count];
    v[0] = (Vertex){xpos, ypos, r, g, b, a, g_info->u0, g_info->v0};
    v[1] = (Vertex){xpos, ypos + h, r, g, b, a, g_info->u0, g_info->v1};
    v[2] = (Vertex){xpos + w, ypos, r, g, b, a, g_info->u1, g_info->v0};
    v[3] = (Vertex){xpos + w, ypos, r, g, b, a, g_info->u1, g_info->v0};
    v[4] = (Vertex){xpos, ypos + h, r, g, b, a, g_info->u0, g_info->v1};
    v[5] = (Vertex){xpos + w, ypos + h, r, g, b, a, g_info->u1, g_info->v1};
    vertex_count += 6;
}

static int utf8_decode(const char* str, unsigned int* codepoint) {
    unsigned char c = (unsigned char)*str;
    if (c < 0x80) {
        *codepoint = c;
        return 1;
    } else if ((c & 0xE0) == 0xC0) {
        *codepoint = ((c & 0x1F) << 6) | ((unsigned char)str[1] & 0x3F);
        return 2;
    } else if ((c & 0xF0) == 0xE0) {
        *codepoint = ((c & 0x0F) << 12) | (((unsigned char)str[1] & 0x3F) << 6) | ((unsigned char)str[2] & 0x3F);
        return 3;
    } else if ((c & 0xF8) == 0xF0) {
        *codepoint = ((c & 0x07) << 18) | (((unsigned char)str[1] & 0x3F) << 12) | (((unsigned char)str[2] & 0x3F) << 6) | ((unsigned char)str[3] & 0x3F);
        return 4;
    }
    *codepoint = 0;
    return 1; // Invalid
}

void vui_render_text(VuiWindow* win, float x, float y, const char* text, float r, float g, float b, float a) {
    float scale = (win->active_font == &win->default_font) ? 2.0f : 1.0f;
    unsigned int codepoint;
    int bytes;
    
    while (*text) {
        bytes = utf8_decode(text, &codepoint);
        vui_render_char(win, x, y, codepoint, r, g, b, a);
        
        int glyph_idx = -1;
        if (win->active_font == &win->default_font) {
            if (codepoint < 128) glyph_idx = codepoint;
        } else {
            if (codepoint < 128) glyph_idx = win->active_font->ascii_map[codepoint];
            else {
                for (int i = 0; i < win->active_font->extra_count; i++) {
                    if (win->active_font->extra_map[i].codepoint == codepoint) {
                        glyph_idx = win->active_font->extra_map[i].index;
                        break;
                    }
                }
            }
        }
        
        if (glyph_idx >= 0) {
            x += win->active_font->glyphs[glyph_idx].advance * scale;
        }
        
        text += bytes;
    }
}

float vui_text_width(VuiWindow* win, const char* text) {
    if (!text || !win->active_font) return 0;
    
    float width = 0;
    float scale = (win->active_font == &win->default_font) ? 2.0f : 1.0f;
    unsigned int codepoint;
    int bytes;
    
    while (*text) {
        bytes = utf8_decode(text, &codepoint);
        
        int glyph_idx = -1;
        if (win->active_font == &win->default_font) {
            if (codepoint < 128) glyph_idx = codepoint;
        } else {
            if (codepoint < 128) glyph_idx = win->active_font->ascii_map[codepoint];
            else {
                for (int i = 0; i < win->active_font->extra_count; i++) {
                    if (win->active_font->extra_map[i].codepoint == codepoint) {
                        glyph_idx = win->active_font->extra_map[i].index;
                        break;
                    }
                }
            }
        }
        
        if (glyph_idx >= 0) {
            width += win->active_font->glyphs[glyph_idx].advance * scale;
        }
        text += bytes;
    }
    return width;
}
