#include "label.h"
#include "../font.h"

void label_get_default_style(ZefirLabelStyle* style) {
    if (!style) return;
    style->font_size = 16.0f;
    style->color[0] = 0.2f;
    style->color[1] = 0.2f;
    style->color[2] = 0.2f;
    style->color[3] = 1.0f;
}

void label_draw(ZefirContext* ctx, const char* text, float x, float y, const ZefirLabelStyle* style) {
    if (!ctx || !text || !style) return;
    
    Font* font = (Font*)zefir_get_font(ctx);
    if (!font) return;
    
    font_render_text(font, text, x, y, style->color[0], style->color[1], style->color[2], style->color[3]);
}