#include "font.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glad/glad.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>

struct Character {
    int width, height;
    int bearing_x, bearing_y;
    int advance;
    float tex_u0, tex_v0, tex_u1, tex_v1;
};

struct Font {
    std::map<unsigned int, Character> chars;
    unsigned int vao, vbo;
    unsigned int texture;
    unsigned int shader;
    unsigned int proj_loc, color_loc;
    float projection[16];
    float size;
    int ascent, descent;
    int atlas_w, atlas_h;
};

static const char* vert_src = R"(
#version 460 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;
out vec2 TexCoord;
uniform mat4 uProjection;
void main() {
    gl_Position = uProjection * vec4(aPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}
)";

static const char* frag_src = R"(
#version 460 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D uTexture;
uniform vec4 uColor;
void main() {
    FragColor = vec4(uColor.rgb, uColor.a * texture(uTexture, TexCoord).r);
}
)";

static bool should_load_char(unsigned int c) {
    if (c >= 32 && c < 128) return true;
    if (c >= 0x0400 && c <= 0x04FF) return true;
    if (c == 0x0490 || c == 0x0491) return true;
    return false;
}

Font* font_create(const char* ttf_path, float size) {
    printf("font_create: %s (%.0f)\n", ttf_path, size);
    
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        printf("FT_Init_FreeType failed\n");
        return nullptr;
    }
    
    FT_Face face;
    if (FT_New_Face(ft, ttf_path, 0, &face)) {
        printf("FT_New_Face failed for %s\n", ttf_path);
        FT_Done_FreeType(ft);
        return nullptr;
    }
    
    Font* f = new Font();
    f->vao = f->vbo = f->texture = f->shader = 0;
    f->proj_loc = f->color_loc = 0;
    f->size = size;
    f->ascent = f->descent = 0;
    f->atlas_w = f->atlas_h = 0;
    memset(f->projection, 0, sizeof(f->projection));
    
    FT_Set_Pixel_Sizes(face, 0, (int)size);
    f->ascent = face->size->metrics.ascender >> 6;
    f->descent = face->size->metrics.descender >> 6;
    
    int total_w = 0, max_h = 0;
    for (unsigned int c = 0; c < 0x0500; c++) {
        if (!should_load_char(c)) continue;
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;
        total_w += face->glyph->bitmap.width;
        if (face->glyph->bitmap.rows > max_h) max_h = face->glyph->bitmap.rows;
    }
    unsigned int extra[] = {0x0490, 0x0491, 0};
    for (int i = 0; extra[i]; i++) {
        if (FT_Load_Char(face, extra[i], FT_LOAD_RENDER)) continue;
        total_w += face->glyph->bitmap.width;
        if (face->glyph->bitmap.rows > max_h) max_h = face->glyph->bitmap.rows;
    }
    
    f->atlas_w = total_w;
    f->atlas_h = max_h;
    
    glGenTextures(1, &f->texture);
    glBindTexture(GL_TEXTURE_2D, f->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, total_w, max_h, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    int x = 0;
    for (unsigned int c = 0; c < 0x0500; c++) {
        if (!should_load_char(c)) continue;
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;
        
        glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0,
                        face->glyph->bitmap.width, face->glyph->bitmap.rows,
                        GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        
        Character ch;
        ch.width = face->glyph->bitmap.width;
        ch.height = face->glyph->bitmap.rows;
        ch.bearing_x = face->glyph->bitmap_left;
        ch.bearing_y = face->glyph->bitmap_top;
        ch.advance = face->glyph->advance.x >> 6;
        ch.tex_u0 = (float)x / total_w;
        ch.tex_v0 = 0.0f;
        ch.tex_u1 = (float)(x + ch.width) / total_w;
        ch.tex_v1 = (float)ch.height / max_h;
        f->chars[c] = ch;
        x += ch.width;
    }
    for (int i = 0; extra[i]; i++) {
        if (FT_Load_Char(face, extra[i], FT_LOAD_RENDER)) continue;
        glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        Character ch;
        ch.width = face->glyph->bitmap.width;
        ch.height = face->glyph->bitmap.rows;
        ch.bearing_x = face->glyph->bitmap_left;
        ch.bearing_y = face->glyph->bitmap_top;
        ch.advance = face->glyph->advance.x >> 6;
        ch.tex_u0 = (float)x / total_w;
        ch.tex_v0 = 0.0f;
        ch.tex_u1 = (float)(x + ch.width) / total_w;
        ch.tex_v1 = (float)ch.height / max_h;
        f->chars[extra[i]] = ch;
        x += ch.width;
    }
    
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vert_src, nullptr);
    glCompileShader(vs);
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &frag_src, nullptr);
    glCompileShader(fs);
    
    f->shader = glCreateProgram();
    glAttachShader(f->shader, vs);
    glAttachShader(f->shader, fs);
    glLinkProgram(f->shader);
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    f->proj_loc = glGetUniformLocation(f->shader, "uProjection");
    f->color_loc = glGetUniformLocation(f->shader, "uColor");
    
    glGenVertexArrays(1, &f->vao);
    glGenBuffers(1, &f->vbo);
    glBindVertexArray(f->vao);
    glBindBuffer(GL_ARRAY_BUFFER, f->vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    printf("Font loaded with FreeType: %s (%.0fpx, %zu chars)\n", ttf_path, size, f->chars.size());
    return f;
}

Font* font_create_default(float size) {
    const char* paths[] = {"C:/Windows/Fonts/segoeui.ttf", "C:/Windows/Fonts/arial.ttf", nullptr};
    for (int i = 0; paths[i]; i++) {
        Font* f = font_create(paths[i], size);
        if (f) return f;
    }
    return nullptr;
}

void font_destroy(Font* f) {
    if (f) {
        glDeleteTextures(1, &f->texture);
        glDeleteProgram(f->shader);
        glDeleteVertexArrays(1, &f->vao);
        glDeleteBuffers(1, &f->vbo);
        delete f;
    }
}

void font_set_projection(Font* f, int w, int h) {
    if (!f) return;
    float proj[16] = {2.0f/w,0,0,0, 0,-2.0f/h,0,0, 0,0,-1,0, -1,1,0,1};
    memcpy(f->projection, proj, sizeof(proj));
}

void font_render_text(Font* f, const char* text, float x, float y, float r, float g, float b, float a) {
    if (!f || !text) return;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glUseProgram(f->shader);
    glUniformMatrix4fv(f->proj_loc, 1, GL_FALSE, f->projection);
    glUniform4f(f->color_loc, r, g, b, a);
    glUniform1i(glGetUniformLocation(f->shader, "uTexture"), 0);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, f->texture);
    glBindVertexArray(f->vao);
    
    float dx = x;
    float dy = y + f->ascent;
    
    int len = strlen(text);
    for (int i = 0; i < len; ) {
        unsigned int cp = 0;
        int cl = 0;
        unsigned char c = text[i];
        if ((c & 0x80) == 0) { cp = c; cl = 1; }
        else if ((c & 0xE0) == 0xC0) { cp = ((c & 0x1F) << 6) | (text[i+1] & 0x3F); cl = 2; }
        else if ((c & 0xF0) == 0xE0) { cp = ((c & 0x0F) << 12) | ((text[i+1] & 0x3F) << 6) | (text[i+2] & 0x3F); cl = 3; }
        else if ((c & 0xF8) == 0xF0) { cp = ((c & 0x07) << 18) | ((text[i+1] & 0x3F) << 12) | ((text[i+2] & 0x3F) << 6) | (text[i+3] & 0x3F); cl = 4; }
        else { cp = '?'; cl = 1; }
        i += cl;
        
        auto it = f->chars.find(cp);
        if (it == f->chars.end()) continue;
        
        Character& ch = it->second;
        float x0 = dx + ch.bearing_x;
        float y0 = dy - ch.bearing_y;
        float x1 = x0 + ch.width;
        float y1 = y0 + ch.height;
        
        float v[] = {
            x0, y0, ch.tex_u0, ch.tex_v0,
            x0, y1, ch.tex_u0, ch.tex_v1,
            x1, y1, ch.tex_u1, ch.tex_v1,
            x0, y0, ch.tex_u0, ch.tex_v0,
            x1, y1, ch.tex_u1, ch.tex_v1,
            x1, y0, ch.tex_u1, ch.tex_v0
        };
        
        glBindBuffer(GL_ARRAY_BUFFER, f->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        dx += ch.advance;
    }
}

float font_get_text_width(Font* f, const char* text) {
    if (!f || !text) return 0;
    float w = 0;
    int len = strlen(text);
    for (int i = 0; i < len; ) {
        unsigned int cp = 0; int cl = 0;
        unsigned char c = text[i];
        if ((c & 0x80) == 0) { cp = c; cl = 1; }
        else if ((c & 0xE0) == 0xC0) { cp = ((c & 0x1F) << 6) | (text[i+1] & 0x3F); cl = 2; }
        else if ((c & 0xF0) == 0xE0) { cp = ((c & 0x0F) << 12) | ((text[i+1] & 0x3F) << 6) | (text[i+2] & 0x3F); cl = 3; }
        else if ((c & 0xF8) == 0xF0) { cp = ((c & 0x07) << 18) | ((text[i+1] & 0x3F) << 12) | ((text[i+2] & 0x3F) << 6) | (text[i+3] & 0x3F); cl = 4; }
        else { cp = '?'; cl = 1; }
        i += cl;
        auto it = f->chars.find(cp);
        if (it != f->chars.end()) w += it->second.advance;
    }
    return w;
}

float font_get_height(Font* f) {
    return f ? (float)(f->ascent - f->descent) : 0;
}