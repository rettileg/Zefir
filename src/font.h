#ifndef FONT_H
#define FONT_H

typedef struct Font Font;

Font* font_create(const char* ttf_path, float size);
Font* font_create_default(float size);
void font_destroy(Font* f);
void font_render_text(Font* f, const char* text, float x, float y, float r, float g, float b, float a);
float font_get_text_width(Font* f, const char* text);
float font_get_height(Font* f);
void font_set_projection(Font* f, int width, int height);

#endif