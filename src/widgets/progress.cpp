#include "progress.h"
#include "../renderer.h"
#include "../font.h"
#include <cstdio>

void progress_get_default_style(ZefirProgressStyle* s) {
    if (!s) return;
    s->height = 22.0f;
    s->border_width = 1.5f;
    s->border_color[0] = 0.5f; s->border_color[1] = 0.5f;
    s->border_color[2] = 0.5f; s->border_color[3] = 1;
    s->bg_color[0] = 0.9f; s->bg_color[1] = 0.9f; s->bg_color[2] = 0.9f; s->bg_color[3] = 1;
    s->fill_color[0] = 0.2f; s->fill_color[1] = 0.55f; s->fill_color[2] = 0.9f; s->fill_color[3] = 1;
    s->corner_radius = 4.0f;
    s->show_percent = true;
    s->font_size = 14.0f;
    s->text_color[0] = 0.1f; s->text_color[1] = 0.1f; s->text_color[2] = 0.1f; s->text_color[3] = 1;
}

void progress_draw(ZefirContext* ctx, float x, float y, float w, float progress, const ZefirProgressStyle* style) {
    if (!ctx || !style) return;
    
    Renderer* r = (Renderer*)zefir_get_renderer(ctx);
    Font* font = (Font*)zefir_get_font(ctx);
    if (!r || !font) return;
    
    if (progress < 0) progress = 0;
    if (progress > 1) progress = 1;
    
    float h = style->height;
    float dummy[4] = {0,0,0,0};
    float highlight[4] = {1,1,1,0.2f};
    float shadow[4] = {0.3f,0.3f,0.3f,0.3f};
    
    renderer_draw_rect_ex(r, x, y, w, h,
                          style->bg_color[0], style->bg_color[1], style->bg_color[2], style->bg_color[3],
                          style->corner_radius, 2, style->border_width, style->border_color, highlight, shadow);
    
    if (progress > 0.001f) {
        float fill_w = (w - 4) * progress;
        if (fill_w < 4) fill_w = 4;
        float fc[4] = {style->fill_color[0], style->fill_color[1], style->fill_color[2], style->fill_color[3]};
        renderer_draw_rect_ex(r, x + 2, y + 2, fill_w, h - 4,
                              fc[0], fc[1], fc[2], fc[3],
                              style->corner_radius - 1, 1, 0, fc, dummy, dummy);
    }

    if (style->show_percent) {
        char buf[8];
        sprintf(buf, "%.0f%%", progress * 100);
        float text_w = font_get_text_width(font, buf);
        float text_h = font_get_height(font);
        float text_x = x + (w - text_w) / 2.0f;
        float text_y = y + (h - text_h) / 2.0f;
        
        font_render_text(font, buf, text_x + 1, text_y + 1, 1, 1, 1, 0.5f);
        font_render_text(font, buf, text_x, text_y, 1, 1, 1, 1);
    }
}