#include "scrollbar.h"
#include "../input.h"
#include "../renderer.h"

void scrollbar_get_default_style(ZefirScrollBarStyle* s) {
    if (!s) return;
    s->track_width = 10.0f;
    s->thumb_min_size = 20.0f;
    s->track_color[0] = 0.85f; s->track_color[1] = 0.85f; s->track_color[2] = 0.85f; s->track_color[3] = 1;
    s->thumb_color[0] = 0.6f; s->thumb_color[1] = 0.6f; s->thumb_color[2] = 0.6f; s->thumb_color[3] = 1;
    s->thumb_hover_color[0] = 0.45f; s->thumb_hover_color[1] = 0.45f; s->thumb_hover_color[2] = 0.45f; s->thumb_hover_color[3] = 1;
    s->border_color[0] = 0.5f; s->border_color[1] = 0.5f; s->border_color[2] = 0.5f; s->border_color[3] = 0.5f;
    s->corner_radius = 5.0f;
}

float scrollbar_v_draw(ZefirContext* ctx, float x, float y, float w, float h, float* scroll, float content_h, float view_h, const ZefirScrollBarStyle* style) {
    if (!ctx || !scroll || !style) return *scroll;
    
    Input* input = (Input*)zefir_get_input(ctx);
    Renderer* r = (Renderer*)zefir_get_renderer(ctx);
    if (!input || !r) return *scroll;
    
    float track_h = h;
    float tw = style->track_width;
    
    float ratio = view_h / content_h;
    if (ratio > 1.0f) ratio = 1.0f;
    float thumb_h = track_h * ratio;
    if (thumb_h < style->thumb_min_size) thumb_h = style->thumb_min_size;
    
    float max_scroll = content_h - view_h;
    if (max_scroll < 0) max_scroll = 0;
    float thumb_y = y + (*scroll / max_scroll) * (track_h - thumb_h);
    if (max_scroll == 0) thumb_y = y;
    
    bool hovered = input_is_hovering(input, x, y, w, h);
    bool hover_thumb = input_is_hovering(input, x, thumb_y, tw, thumb_h);
    
    static bool dragging = false;
    static float drag_start_y = 0;
    static float scroll_start = 0;
    
    if (hover_thumb && input_mouse_pressed(input)) {
        dragging = true;
        float mx, my;
        input_get_mouse(input, &mx, &my);
        drag_start_y = my;
        scroll_start = *scroll;
    }
    
    if (dragging) {
        if (input_mouse_down(input)) {
            float mx, my;
            input_get_mouse(input, &mx, &my);
            float dy = my - drag_start_y;
            *scroll = scroll_start + (dy / (track_h - thumb_h)) * max_scroll;
        } else {
            dragging = false;
        }
    }
    
    if (hovered && !hover_thumb && input_mouse_pressed(input)) {
        float mx, my;
        input_get_mouse(input, &mx, &my);
        float target = ((my - y - thumb_h/2) / (track_h - thumb_h)) * max_scroll;
        *scroll = target;
    }
    
    if (*scroll < 0) *scroll = 0;
    if (*scroll > max_scroll) *scroll = max_scroll;
    
    float dummy[4] = {0,0,0,0};
    renderer_draw_rect_ex(r, x, y, tw, track_h, 
                          style->track_color[0], style->track_color[1], style->track_color[2], style->track_color[3],
                          style->corner_radius, 2, 1.0f, style->border_color, dummy, dummy);
    
    float tc[4] = {style->thumb_color[0], style->thumb_color[1], style->thumb_color[2], style->thumb_color[3]};
    if (hover_thumb || dragging) {
        tc[0] = style->thumb_hover_color[0]; tc[1] = style->thumb_hover_color[1];
        tc[2] = style->thumb_hover_color[2]; tc[3] = style->thumb_hover_color[3];
    }
    
    renderer_draw_rect_ex(r, x + 1, thumb_y, tw - 2, thumb_h,
                          tc[0], tc[1], tc[2], tc[3],
                          style->corner_radius - 1, 1, 1.0f, style->border_color, dummy, dummy);
    
    return *scroll;
}

float scrollbar_h_draw(ZefirContext* ctx, float x, float y, float w, float h, float* scroll, float content_w, float view_w, const ZefirScrollBarStyle* style) {
    if (!ctx || !scroll || !style) return *scroll;
    
    Input* input = (Input*)zefir_get_input(ctx);
    Renderer* r = (Renderer*)zefir_get_renderer(ctx);
    if (!input || !r) return *scroll;
    
    float track_w = w;
    float th = style->track_width;
    
    float ratio = view_w / content_w;
    if (ratio > 1.0f) ratio = 1.0f;
    float thumb_w = track_w * ratio;
    if (thumb_w < style->thumb_min_size) thumb_w = style->thumb_min_size;
    
    float max_scroll = content_w - view_w;
    if (max_scroll < 0) max_scroll = 0;
    float thumb_x = x + (*scroll / max_scroll) * (track_w - thumb_w);
    if (max_scroll == 0) thumb_x = x;
    
    bool hovered = input_is_hovering(input, x, y, w, h);
    bool hover_thumb = input_is_hovering(input, thumb_x, y, thumb_w, th);
    
    static bool dragging = false;
    static float drag_start_x = 0, scroll_start = 0;
    
    if (hover_thumb && input_mouse_pressed(input)) {
        dragging = true;
        float mx, my;
        input_get_mouse(input, &mx, &my);
        drag_start_x = mx;
        scroll_start = *scroll;
    }
    
    if (dragging) {
        if (input_mouse_down(input)) {
            float mx, my;
            input_get_mouse(input, &mx, &my);
            float dx = mx - drag_start_x;
            *scroll = scroll_start + (dx / (track_w - thumb_w)) * max_scroll;
        } else {
            dragging = false;
        }
    }
    
    if (*scroll < 0) *scroll = 0;
    if (*scroll > max_scroll) *scroll = max_scroll;
    
    float dummy[4] = {0,0,0,0};
    renderer_draw_rect_ex(r, x, y, track_w, th,
                          style->track_color[0], style->track_color[1], style->track_color[2], style->track_color[3],
                          style->corner_radius, 2, 1.0f, style->border_color, dummy, dummy);
    
    float tc[4] = {style->thumb_color[0], style->thumb_color[1], style->thumb_color[2], style->thumb_color[3]};
    if (hover_thumb || dragging) {
        tc[0] = style->thumb_hover_color[0]; tc[1] = style->thumb_hover_color[1];
        tc[2] = style->thumb_hover_color[2]; tc[3] = style->thumb_hover_color[3];
    }
    
    renderer_draw_rect_ex(r, thumb_x, y + 1, thumb_w, th - 2,
                          tc[0], tc[1], tc[2], tc[3],
                          style->corner_radius - 1, 1, 1.0f, style->border_color, dummy, dummy);
    
    return *scroll;
}