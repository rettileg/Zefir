#include "zf_menu.h"
#include "../input.h"
#include "../renderer.h"
#include "../font.h"
#include "button.h"

struct MenuState {
    bool bar_open;
    float cursor_x;
};

static MenuState g_menu;

void zefir_menu_bar_begin(ZefirContext* ctx) {
    if (!ctx) return;
    g_menu.bar_open = true;
    g_menu.cursor_x = 2.0f;
    
    const float* bg = zefir_get_bg_color(ctx);
    Renderer* r = (Renderer*)zefir_get_renderer(ctx);
    int ww, wh;
    zefir_get_size(ctx, &ww, &wh);
    
    float bar_h = 32.0f;
    float bar_bg[4] = {bg[0]*0.94f, bg[1]*0.94f, bg[2]*0.94f, 1.0f};
    float border[4] = {0.5f, 0.5f, 0.5f, 0.3f};
    float dummy[4] = {0,0,0,0};
    renderer_draw_rect_ex(r, 0, 0, (float)ww, bar_h, bar_bg[0], bar_bg[1], bar_bg[2], 1, 0, 0, 1.0f, border, dummy, dummy);
}

bool zefir_menu_item(ZefirContext* ctx, const char* text) {
    if (!ctx || !g_menu.bar_open) return false;
    
    Font* font = (Font*)zefir_get_font(ctx);
    if (!font) return false;
    
    float text_w = font_get_text_width(font, text) + 20;
    float x = g_menu.cursor_x;
    float y = 2.0f;
    float h = 28.0f;
    g_menu.cursor_x += text_w + 2;
    
    ZefirButtonStyle style;
    button_get_default_style(&style);
    style.text = text;
    style.text_color[0] = 0.1f;
    style.text_color[1] = 0.1f;
    style.text_color[2] = 0.1f;
    style.text_color[3] = 1.0f;
    style.font_size = 15.0f;
    style.corner_radius = 4.0f;
    style.border_width = 0;
    style.press_offset = 0;
    style.hover_brightness = 1.0f;
    style.press_darkness = 0.05f;
    
    return button_draw(ctx, x, y, text_w, h, &style);
}

void zefir_menu_bar_end(ZefirContext* ctx) {
    g_menu.bar_open = false;
}