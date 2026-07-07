#include "tooltip.h"
#include "../renderer.h"
#include "../font.h"
#include "../input.h"
#include <cstring>
#include <cmath>

struct TooltipState {
    char text[256];
    float pos_x, pos_y;
    float cur_x, cur_y, cur_w, cur_h;
    float target_w, target_h;
    float alpha;
    float timer;
    float last_mx, last_my;
    bool visible;
    bool was_visible;
};

static TooltipState g_tip;

void tooltip_draw(ZefirContext* ctx, const char* text, float x, float y) {
    if (!ctx || !text) {
        g_tip.visible = false;
        g_tip.alpha = 0;
        return;
    }
    
    Input* input = (Input*)zefir_get_input(ctx);
    Font* font = (Font*)zefir_get_font(ctx);
    Renderer* r = (Renderer*)zefir_get_renderer(ctx);
    if (!input || !font || !r) return;
    
    float mx, my;
    input_get_mouse(input, &mx, &my);
    
    bool mouse_moved = (fabsf(mx - g_tip.last_mx) > 0.3f || fabsf(my - g_tip.last_my) > 0.3f);
    g_tip.last_mx = mx;
    g_tip.last_my = my;
    
    strcpy(g_tip.text, text);
    
    if (mouse_moved) {
        g_tip.timer = 0;
        g_tip.alpha = 0;
        g_tip.visible = false;
        g_tip.cur_w = 0;
        g_tip.cur_h = 0;
        g_tip.pos_x = mx;
        g_tip.pos_y = my;
        g_tip.was_visible = false;
        return;
    }
    
    g_tip.timer += 0.016f;
    
    if (g_tip.timer >= 0.4f) {
        if (!g_tip.was_visible) {
            g_tip.pos_x = mx;
            g_tip.pos_y = my;
            g_tip.was_visible = true;
            g_tip.cur_x = mx + 10;
            g_tip.cur_y = my + 18;
        }
        g_tip.visible = true;
        g_tip.alpha += 0.016f * 5.0f;
        if (g_tip.alpha > 1.0f) g_tip.alpha = 1.0f;
    }
    
    if (!g_tip.visible) return;
    
    float text_w = font_get_text_width(font, text);
    float text_h = font_get_height(font);
    float pad = 5;
    g_tip.target_w = text_w + pad * 2;
    g_tip.target_h = text_h + pad * 2;
    
    float target_x = g_tip.pos_x + 10;
    float target_y = g_tip.pos_y + 18;
    
    int ww, wh;
    zefir_get_size(ctx, &ww, &wh);
    if (target_x + g_tip.target_w > ww) target_x = g_tip.pos_x - g_tip.target_w - 10;
    if (target_y + g_tip.target_h > wh) target_y = g_tip.pos_y - g_tip.target_h - 6;
    
    g_tip.cur_x += (target_x - g_tip.cur_x) * 0.2f;
    g_tip.cur_y += (target_y - g_tip.cur_y) * 0.2f;
    g_tip.cur_w += (g_tip.target_w - g_tip.cur_w) * 0.3f;
    g_tip.cur_h += (g_tip.target_h - g_tip.cur_h) * 0.3f;
    
    float bg[4] = {0.27f, 0.27f, 0.27f, g_tip.alpha * 0.93f};
    float border[4] = {0.45f, 0.45f, 0.45f, g_tip.alpha * 0.5f};
    float dummy[4] = {0,0,0,0};
    
    renderer_draw_rect_ex(r, g_tip.cur_x, g_tip.cur_y, g_tip.cur_w, g_tip.cur_h,
                          bg[0], bg[1], bg[2], bg[3], 3, 0, 0, border, dummy, dummy);
    
    float tc[4] = {0.95f, 0.95f, 0.95f, g_tip.alpha};
    font_render_text(font, text, g_tip.cur_x + pad, g_tip.cur_y + pad, tc[0], tc[1], tc[2], tc[3]);
}