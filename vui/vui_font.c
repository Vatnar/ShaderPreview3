#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/gl.h>
#include <stdio.h>

#include "vui.h"
#include "vui_private.h"

local_persist FT_Library ft;
local_persist int ft_initialized = 0;

void 
vui_font_init(void)
{
    if(!ft_initialized) {
        if(FT_Init_FreeType(&ft)) {
            fprintf(stderr, "Could not init FreeType Library\n");
            return;
        }
        ft_initialized = 1;
    }
}

VuiFont* 
vui_load_font(VuiWindow* window, const char* filename, int size)
{
    if(!ft_initialized) vui_font_init();
    if(!ft_initialized) return NULL;

    FT_Face face;
    if(FT_New_Face(ft, filename, 0, &face)) {
        fprintf(stderr, "Failed to load font: %s\n", filename);
        return NULL;
    }
    printf("Font loaded: %s\n", filename);

    FT_Set_Pixel_Sizes(face, 0, size);

    VuiFont* font = arena_push(window->arena, sizeof(VuiFont), 1);
    font->size = size;
    font->height = size;
    
    font->glyph_capacity = 256;
    font->glyphs = arena_push(window->arena, font->glyph_capacity * sizeof(VuiGlyph), 1);
    
    for(int i=0; i<128; i++) font->ascii_map[i] = -1;
    
    font->extra_capacity = 64;
    font->extra_map = arena_push(window->arena, font->extra_capacity * sizeof(*font->extra_map), 1);

    int tex_w = 512;
    int tex_h = 512;
    U8* tex_data = arena_push(window->arena, tex_w * tex_h, 1);

    int x = 0;
    int y = 0;
    int row_h = 0;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    U32 icons[] = { 
        0xF04B, 0xF04C, 0xF04D, 0xF051, 0xF0E2, 0xF0C7, 0xF07C, 0xF013, 0xF120,
        0xE62B,
        0
    };

    for(U32 c = 32; c < 128; c++) {
        if(FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;
        
        if(x + face->glyph->bitmap.width >= tex_w) {
            x = 0;
            y += row_h + 1;
            row_h = 0;
        }
        if(face->glyph->bitmap.rows > row_h) row_h = face->glyph->bitmap.rows;
        if(y + row_h >= tex_h) break;

        for(int r = 0; r < face->glyph->bitmap.rows; r++) {
            for(int col = 0; col < face->glyph->bitmap.width; col++) {
                tex_data[(y + r) * tex_w + (x + col)] = 
                    face->glyph->bitmap.buffer[r * face->glyph->bitmap.width + col];
            }
        }

        VuiGlyph glyph;
        glyph.u0 = (F32)x / tex_w;
        glyph.v0 = (F32)y / tex_h;
        glyph.u1 = (F32)(x + face->glyph->bitmap.width) / tex_w;
        glyph.v1 = (F32)(y + face->glyph->bitmap.rows) / tex_h;
        glyph.width = face->glyph->bitmap.width;
        glyph.height = face->glyph->bitmap.rows;
        glyph.bearing_x = face->glyph->bitmap_left;
        glyph.bearing_y = face->glyph->bitmap_top;
        glyph.advance = face->glyph->advance.x >> 6;

        font->glyphs[font->glyph_count] = glyph;
        font->ascii_map[c] = font->glyph_count;
        font->glyph_count++;

        x += face->glyph->bitmap.width + 1;
    }

    for(int i = 0; icons[i] != 0; i++) {
        if(FT_Load_Char(face, icons[i], FT_LOAD_RENDER)) continue;
        
        if(x + face->glyph->bitmap.width >= tex_w) {
            x = 0;
            y += row_h + 1;
            row_h = 0;
        }
        if(face->glyph->bitmap.rows > row_h) row_h = face->glyph->bitmap.rows;
        if(y + row_h >= tex_h) break;

        for(int r = 0; r < face->glyph->bitmap.rows; r++) {
            for(int col = 0; col < face->glyph->bitmap.width; col++) {
                tex_data[(y + r) * tex_w + (x + col)] = 
                    face->glyph->bitmap.buffer[r * face->glyph->bitmap.width + col];
            }
        }

        VuiGlyph glyph;
        glyph.u0 = (F32)x / tex_w;
        glyph.v0 = (F32)y / tex_h;
        glyph.u1 = (F32)(x + face->glyph->bitmap.width) / tex_w;
        glyph.v1 = (F32)(y + face->glyph->bitmap.rows) / tex_h;
        glyph.width = face->glyph->bitmap.width;
        glyph.height = face->glyph->bitmap.rows;
        glyph.bearing_x = face->glyph->bitmap_left;
        glyph.bearing_y = face->glyph->bitmap_top;
        glyph.advance = face->glyph->advance.x >> 6;

        font->glyphs[font->glyph_count] = glyph;
        font->extra_map[font->extra_count].codepoint = icons[i];
        font->extra_map[font->extra_count].index = font->glyph_count;
        font->extra_count++;
        font->glyph_count++;

        x += face->glyph->bitmap.width + 1;
    }

    FT_Done_Face(face);

    U32 texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, tex_w, tex_h, 0, GL_RED, GL_UNSIGNED_BYTE, tex_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    font->texture_id = texture_id;

    return font;
}

void vui_set_font(VuiWindow* window, VuiFont* font) {
    if (font) {
        window->active_font = font;
    } else {
        window->active_font = &window->default_font;
    }
}

