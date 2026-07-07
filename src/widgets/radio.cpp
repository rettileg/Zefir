#include "radio.h"
#include "../input.h"
#include "../renderer.h"
#include "../font.h"

void radio_get_default_style(ZefirRadioStyle* s) {
    if (!s) return;
    s->circle_size = 18.0f;
    s->dot_radius = 5.0f;
    s->border_width = 2.0f;
    s->border_color[0] = 0.45f; s->border_color[1] = 0.45f;
    s->border_color[2] = 0.45f; s->border_color[3] = 1;
    s->check_color[0] = 0.2f; s->check_color[1] = 0.5f;
    s->check_color[2] = 0.9f; s->check_color[3] = 1;
    s->hover_color[0] = 0.6f; s->hover_color[1] = 0.6f;
    s->hover_color[2] = 0.6f; s->hover_color[3] = 0.3f;
    s->corner_radius = 9.0f;
    s->text = nullptr;
    s->font_size = 16.0f;
    s->text_color[0] = 0.15f; s->text_color[1] = 0.15f;
    s->text_color[2] = 0.15f; s->text_color[3] = 1;
}

bool radio_draw(ZefirContext* ctx, float x, float y, float size, const char* text, int* selected, int index, const ZefirRadioStyle* style) {
    if (!ctx || !selected || !style) return *selected == index;
    
    Input* input = (Input*)zefir_get_input(ctx);
    Renderer* r = (Renderer*)zefir_get_renderer(ctx);
    Font* font = (Font*)zefir_get_font(ctx);
    const float* bg = zefir_get_bg_color(ctx);
    if (!input || !r || !bg) return *selected == index;
    
    float s = size > 0 ? size : style->circle_size;
    bool hovered = input_is_hovering(input, x, y, s, s);
    bool clicked = hovered && input_mouse_released(input);
    
    if (clicked) *selected = index;
    
    bool active = (*selected == index);
    
    float highlight[4] = {1,1,1,0.3f};
    float shadow[4] = {0.3f,0.3f,0.3f,0.4f};
    float dummy[4] = {0,0,0,0};
    
    renderer_draw_rect_ex(r, x, y, s, s,
                          bg[0], bg[1], bg[2], 1.0f,
                          style->corner_radius, active ? 2 : 1,
                          style->border_width, style->border_color, highlight, shadow);
    
    if (active) {
        float dot_size = style->dot_radius * 2;
        float dot_x = x + s/2 - dot_size/2;
        float dot_y = y + s/2 - dot_size/2;
        float chk[4] = {style->check_color[0], style->check_color[1], style->check_color[2], style->check_color[3]};
        renderer_draw_rect_ex(r, dot_x, dot_y, dot_size, dot_size,
                              chk[0], chk[1], chk[2], chk[3],
                              style->dot_radius, 0, 0, chk, dummy, dummy);
    }
    
    if (text && font) {
        font_render_text(font, text, x + s + 8, y + (s - font_get_height(font))/2,
                        style->text_color[0], style->text_color[1], style->text_color[2], style->text_color[3]);
    }
    
    return active;
}