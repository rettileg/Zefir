#include "slider.h"
#include "../input.h"
#include "../renderer.h"
#include "../font.h"
#include <cmath>
#include <cstdio>

void slider_get_default_style(ZefirSliderStyle* style) {
    if (!style) return;
    style->track_height = 6.0f;
    style->handle_radius = 10.0f;
    style->track_color[0] = 0.6f; style->track_color[1] = 0.6f;
    style->track_color[2] = 0.6f; style->track_color[3] = 1.0f;
    style->fill_color[0] = 0.3f; style->fill_color[1] = 0.5f;
    style->fill_color[2] = 0.9f; style->fill_color[3] = 1.0f;
    style->handle_color[0] = 0.9f; style->handle_color[1] = 0.9f;
    style->handle_color[2] = 0.9f; style->handle_color[3] = 1.0f;
    style->border_width = 1.5f;
    style->border_color[0] = 0.5f; style->border_color[1] = 0.5f;
    style->border_color[2] = 0.5f; style->border_color[3] = 1.0f;
    style->show_value = true;
    style->font_size = 14.0f;
}

float slider_draw(ZefirContext* ctx, float x, float y, float w, float h, float value, float min_val, float max_val, const ZefirSliderStyle* style) {
    if (!ctx || !style) return value;
    
    Input* input = (Input*)zefir_get_input(ctx);
    Renderer* renderer = (Renderer*)zefir_get_renderer(ctx);
    Font* font = (Font*)zefir_get_font(ctx);
    const float* bg = zefir_get_bg_color(ctx);
    if (!input || !renderer || !bg) return value;
    
    bool hovered = input_is_hovering(input, x, y, w, h);
    bool dragging = hovered && input_mouse_down(input);
    
    if (dragging || (hovered && input_mouse_pressed(input))) {
        float mx, my;
        input_get_mouse(input, &mx, &my);
        float t = (mx - x) / w;
        if (t < 0) t = 0;
        if (t > 1) t = 1;
        value = min_val + t * (max_val - min_val);
    }
    
    float t = (value - min_val) / (max_val - min_val);
    float track_y = y + h / 2.0f - style->track_height / 2.0f;
    float handle_x = x + t * w - style->handle_radius;
    float handle_y = y + h / 2.0f - style->handle_radius;
    float handle_size = style->handle_radius * 2.0f;
    
    float highlight[4] = {1,1,1,0.3f};
    float shadow[4] = {0.3f,0.3f,0.3f,0.5f};
    renderer_draw_rect_ex(renderer, x, track_y, w, style->track_height,
                          style->track_color[0], style->track_color[1], style->track_color[2], style->track_color[3],
                          2.0f, 2, 1.0f, style->border_color, highlight, shadow);
    
    if (t > 0.01f) {
        renderer_draw_rect_ex(renderer, x, track_y, w * t, style->track_height,
                              style->fill_color[0], style->fill_color[1], style->fill_color[2], style->fill_color[3],
                              2.0f, 0, 0, style->fill_color, highlight, shadow);
    }
    
    float hc[4] = {style->handle_color[0], style->handle_color[1], style->handle_color[2], style->handle_color[3]};
    renderer_draw_rect_ex(renderer, handle_x, handle_y, handle_size, handle_size,
                          hc[0], hc[1], hc[2], hc[3],
                          style->handle_radius, (dragging || hovered) ? 2 : 1,
                          style->border_width, style->border_color, highlight, shadow);
    
    if (style->show_value && font) {
        char buf[32];
        sprintf(buf, "%.1f", value);
        float text_w = font_get_text_width(font, buf);
        float text_y = y - font_get_height(font) - 4;
        font_render_text(font, buf, x + w / 2.0f - text_w / 2.0f, text_y, 0.9f, 0.9f, 0.9f, 1.0f);
    }
    
    return value;
}