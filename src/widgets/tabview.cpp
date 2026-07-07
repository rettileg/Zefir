#include "tabview.h"
#include "../input.h"
#include "../renderer.h"
#include "../font.h"

void tabview_get_default_style(ZefirTabViewStyle* s) {
    if (!s) return;
    s->tab_height = 32.0f;
    s->font_size = 15.0f;
    s->text_color[0] = 0.15f; s->text_color[1] = 0.15f; s->text_color[2] = 0.15f; s->text_color[3] = 1;
    s->active_tab_color[0] = 1; s->active_tab_color[1] = 1; s->active_tab_color[2] = 1; s->active_tab_color[3] = 1;
    s->inactive_tab_color[0] = 0.88f; s->inactive_tab_color[1] = 0.88f; s->inactive_tab_color[2] = 0.88f; s->inactive_tab_color[3] = 1;
    s->border_color[0] = 0.5f; s->border_color[1] = 0.5f; s->border_color[2] = 0.5f; s->border_color[3] = 1;
    s->corner_radius = 4.0f;
    s->spacing = 1.0f;
}

int tabview_draw(ZefirContext* ctx, float x, float y, float w, float h, const char** tabs, int tab_count, int* active_tab, const ZefirTabViewStyle* style) {
    if (!ctx || !tabs || !active_tab || !style) return *active_tab;
    
    Input* input = (Input*)zefir_get_input(ctx);
    Font* font = (Font*)zefir_get_font(ctx);
    Renderer* r = (Renderer*)zefir_get_renderer(ctx);
    const float* bg = zefir_get_bg_color(ctx);
    if (!input || !font || !r || !bg) return *active_tab;
    
    float th = style->tab_height;
    float tab_x = x + 2;
    float dummy[4] = {0,0,0,0};
    float highlight[4] = {1,1,1,0.2f};
    float shadow[4] = {0.3f,0.3f,0.3f,0.3f};
    
    for (int i = 0; i < tab_count; i++) {
        float text_w = font_get_text_width(font, tabs[i]) + 20;
        bool hovered = input_is_hovering(input, tab_x, y, text_w, th);
        bool clicked = hovered && input_mouse_released(input);
        
        if (clicked) {
            *active_tab = i;
        }
        
        bool active = (*active_tab == i);
        
        if (active) {
            renderer_draw_rect_ex(r, tab_x, y - 1, text_w, th + 2,
                                  style->active_tab_color[0], style->active_tab_color[1],
                                  style->active_tab_color[2], style->active_tab_color[3],
                                  style->corner_radius, 1, 1.0f, style->border_color, highlight, shadow);
        } else if (hovered) {
            float hc[4] = {bg[0]*0.95f, bg[1]*0.95f, bg[2]*0.95f, 1};
            renderer_draw_rect_ex(r, tab_x, y + 2, text_w, th - 2,
                                  hc[0], hc[1], hc[2], hc[3],
                                  style->corner_radius, 0, 0, style->border_color, dummy, dummy);
        } else {
            renderer_draw_rect_ex(r, tab_x, y + 2, text_w, th - 2,
                                  style->inactive_tab_color[0], style->inactive_tab_color[1],
                                  style->inactive_tab_color[2], style->inactive_tab_color[3],
                                  style->corner_radius, 0, 0, style->border_color, dummy, dummy);
        }
        
        font_render_text(font, tabs[i], tab_x + 10, y + (th - font_get_height(font))/2,
                        style->text_color[0], style->text_color[1], style->text_color[2], style->text_color[3]);
        
        tab_x += text_w + style->spacing;
    }
    
    float line_y = y + th;
    float line_h = 1.5f;
    float lc[4] = {0.5f, 0.5f, 0.5f, 0.6f};
    renderer_draw_rect_ex(r, x, line_y, w, line_h, lc[0], lc[1], lc[2], lc[3], 0, 0, 0, lc, dummy, dummy);
    
    return *active_tab;
}