#include "window.h"
#include "../input.h"
#include "../renderer.h"
#include "../font.h"
#include <cstring>

struct ZefirGuiWindow {
    char title[128];
    float x, y, w, h;
    bool dragging;
    float drag_off_x, drag_off_y;
    bool resizing;
    float resize_start_w, resize_start_h;
    float resize_start_x, resize_start_y;
    bool collapsed;
    bool closed;
    float collapsed_h;
};

ZefirGuiWindow* zefir_window_create(const char* title, float x, float y, float w, float h) {
    ZefirGuiWindow* win = new ZefirGuiWindow();
    strncpy(win->title, title, 127);
    win->title[127] = 0;
    win->x = x; win->y = y; win->w = w; win->h = h;
    win->dragging = false;
    win->resizing = false;
    win->collapsed = false;
    win->closed = false;
    win->collapsed_h = 28.0f;
    return win;
}

void zefir_window_destroy(ZefirGuiWindow* win) {
    delete win;
}

bool zefir_window_begin(ZefirGuiWindow* win, ZefirContext* ctx) {
    if (!win || !ctx || win->closed) return false;
    
    Input* input = (Input*)zefir_get_input(ctx);
    Renderer* r = (Renderer*)zefir_get_renderer(ctx);
    Font* font = (Font*)zefir_get_font(ctx);
    const float* bg = zefir_get_bg_color(ctx);
    if (!input || !r || !font || !bg) return false;
    
    float title_h = 28.0f;
    float dummy[4] = {0,0,0,0};
    float shadow[4] = {0.15f, 0.15f, 0.15f, 0.4f};
    float highlight[4] = {1,1,1,0.2f};
    float border[4] = {0.5f, 0.5f, 0.5f, 0.8f};
    
    float current_h = win->collapsed ? win->collapsed_h : win->h;
    
    bool hover_title = input_is_hovering(input, win->x, win->y, win->w, title_h);
    
    if (hover_title && input_mouse_pressed(input)) {
        win->dragging = true;
        float mx, my;
        input_get_mouse(input, &mx, &my);
        win->drag_off_x = mx - win->x;
        win->drag_off_y = my - win->y;
    }
    
    if (win->dragging) {
        if (input_mouse_down(input)) {
            float mx, my;
            input_get_mouse(input, &mx, &my);
            win->x = mx - win->drag_off_x;
            win->y = my - win->drag_off_y;
        } else {
            win->dragging = false;
        }
    }
    
    if (!win->collapsed) {
        float resize_zone = 12;
        bool hover_resize = input_is_hovering(input, win->x + win->w - resize_zone, 
                                              win->y + win->h - resize_zone, resize_zone, resize_zone);
        
        if (hover_resize && input_mouse_pressed(input)) {
            win->resizing = true;
            float mx, my;
            input_get_mouse(input, &mx, &my);
            win->resize_start_w = win->w;
            win->resize_start_h = win->h;
            win->resize_start_x = mx;
            win->resize_start_y = my;
        }
        
        if (win->resizing) {
            if (input_mouse_down(input)) {
                float mx, my;
                input_get_mouse(input, &mx, &my);
                win->w = win->resize_start_w + (mx - win->resize_start_x);
                win->h = win->resize_start_h + (my - win->resize_start_y);
                if (win->w < 100) win->w = 100;
                if (win->h < 60) win->h = 60;
            } else {
                win->resizing = false;
            }
        }
    }
    
    float shadow_bg[4] = {0.1f, 0.1f, 0.1f, 0.3f};

    renderer_draw_rect_ex(r, win->x + 4, win->y + 4, win->w, current_h,
                          shadow_bg[0], shadow_bg[1], shadow_bg[2], shadow_bg[3], 6, 0, 0, dummy, dummy, dummy);
    
    float win_bg[4] = {bg[0], bg[1], bg[2], 1};

    renderer_draw_rect_ex(r, win->x, win->y, win->w, current_h,
                          win_bg[0], win_bg[1], win_bg[2], win_bg[3], 6, 1, 1.0f, border, highlight, shadow);

    float title_bg[4] = {win->collapsed ? 0.15f : 0.2f, 
                          win->collapsed ? 0.35f : 0.45f, 
                          win->collapsed ? 0.7f : 0.9f, 1};

    renderer_draw_rect_ex(r, win->x, win->y, win->w, title_h,
                          title_bg[0], title_bg[1], title_bg[2], title_bg[3], 6, 0, 0, border, dummy, dummy);
    
    float tc[4] = {1, 1, 1, 1};
    font_render_text(font, win->title, win->x + 10, win->y + 5, tc[0], tc[1], tc[2], tc[3]);
    
    float min_x = win->x + win->w - 50;
    float min_y = win->y + 4;
    bool hover_min = input_is_hovering(input, min_x, min_y, 20, 20);
    
    if (hover_min) {
        float hc[4] = {0.3f, 0.3f, 0.3f, 0.5f};
        renderer_draw_rect_ex(r, min_x, min_y, 20, 20, hc[0], hc[1], hc[2], hc[3], 4, 0, 0, hc, dummy, dummy);
    }
    font_render_text(font, win->collapsed ? "+" : "-", min_x + 5, min_y + 2, 1, 1, 1, 1);
    
    if (hover_min && input_mouse_released(input)) {
        win->collapsed = !win->collapsed;
    }
    
    float close_x = win->x + win->w - 24;
    float close_y = win->y + 4;
    bool hover_close = input_is_hovering(input, close_x, close_y, 20, 20);
    
    if (hover_close) {
        float cb[4] = {0.9f, 0.2f, 0.2f, 0.8f};
        renderer_draw_rect_ex(r, close_x, close_y, 20, 20, cb[0], cb[1], cb[2], cb[3], 4, 0, 0, cb, dummy, dummy);
    }
    font_render_text(font, "X", close_x + 5, close_y + 2, 1, 1, 1, 1);
    
    if (hover_close && input_mouse_released(input)) {
        win->closed = true;
        return false;
    }

    if (win->collapsed) return false;
    
    return true;
}

void zefir_window_end(ZefirGuiWindow* win, ZefirContext* ctx) {
    // N
}

void zefir_window_get_pos(ZefirGuiWindow* win, float* x, float* y) {
    if (win) { *x = win->x; *y = win->y; }
}

void zefir_window_get_size(ZefirGuiWindow* win, float* w, float* h) {
    if (win) { *w = win->w; *h = win->h; }
}