#include "button.h"
#include "../input.h"
#include "../renderer.h"
#include "../font.h"
#include <cmath>
#include <cstring>

struct ButtonAnimState {
    double press_time;
    float anim_progress;
    bool was_pressed;
};

static ButtonAnimState g_button_anim = {0.0, 0.0f, false};

void button_get_default_style(ZefirButtonStyle* style) {
    if (!style) return;
    
    style->border_width = 1.5f;
    style->border_color[0] = 0.70f; style->border_color[1] = 0.67f;
    style->border_color[2] = 0.63f; style->border_color[3] = 1.0f;
    
    style->highlight_color[0] = 1.0f; style->highlight_color[1] = 1.0f;
    style->highlight_color[2] = 1.0f; style->highlight_color[3] = 0.95f;
    
    style->shadow_color[0] = 0.50f; style->shadow_color[1] = 0.47f;
    style->shadow_color[2] = 0.43f; style->shadow_color[3] = 0.9f;
    
    style->corner_radius = 3.0f;
    style->press_offset = 2.0f;
    style->hover_brightness = 1.02f;
    style->press_darkness = 0.08f;
    style->anim_speed_press = 0.05f;
    style->anim_speed_release = 0.1f;
    
    style->text = nullptr;
    style->font_size = 16.0f;
    style->text_color[0] = 0.2f;
    style->text_color[1] = 0.2f;
    style->text_color[2] = 0.2f;
    style->text_color[3] = 1.0f;
}

bool button_draw(ZefirContext* ctx, float x, float y, float w, float h, const ZefirButtonStyle* style) {
    if (!ctx || !style) return false;
    
    Input* input = (Input*)zefir_get_input(ctx);
    Renderer* renderer = (Renderer*)zefir_get_renderer(ctx);
    Font* font = (Font*)zefir_get_font(ctx);
    const float* bg_color = zefir_get_bg_color(ctx);
    
    if (!input || !renderer || !bg_color) return false;
    
    bool hovered = input_is_hovering(input, x, y, w, h);
    bool pressed = hovered && input_mouse_down(input);
    bool clicked = hovered && input_mouse_released(input);
    
    double current_time = zefir_get_time(ctx);
    
    if (pressed && !g_button_anim.was_pressed) {
        g_button_anim.press_time = current_time;
    }
    
    if (pressed) {
        float elapsed = (float)(current_time - g_button_anim.press_time);
        g_button_anim.anim_progress = fminf(elapsed / style->anim_speed_press, 1.0f);
    } else if (g_button_anim.anim_progress > 0.0f) {
        float elapsed = (float)(current_time - g_button_anim.press_time);
        g_button_anim.anim_progress = fmaxf(1.0f - elapsed / style->anim_speed_release, 0.0f);
    }
    
    g_button_anim.was_pressed = pressed;
    
    float press_offset = g_button_anim.anim_progress * style->press_offset;
    float draw_x = x + press_offset;
    float draw_y = y + press_offset;
    
    float red = bg_color[0];
    float green = bg_color[1];
    float blue = bg_color[2];
    
    if (pressed || g_button_anim.anim_progress > 0.5f) {
        float darken = 1.0f - g_button_anim.anim_progress * style->press_darkness;
        red *= darken; green *= darken; blue *= darken;
    } else if (hovered) {
        red *= style->hover_brightness;
        green *= style->hover_brightness;
        blue *= style->hover_brightness;
    }
    
    int style_type = (pressed || g_button_anim.anim_progress > 0.3f) ? 2 : 1;
    
    renderer_draw_rect_ex(renderer, draw_x, draw_y, w, h,
                          red, green, blue, 1.0f,
                          style->corner_radius, style_type,
                          style->border_width,
                          style->border_color,
                          style->highlight_color,
                          style->shadow_color);
    
    if (style->text && font) {
        float text_w = font_get_text_width(font, style->text);
        float text_h = font_get_height(font);
        
        float text_x = draw_x + (w - text_w) / 2.0f;
        float text_y = draw_y + (h - text_h) / 2.0f;
        
        font_render_text(font, style->text, text_x, text_y,
                        style->text_color[0], style->text_color[1],
                        style->text_color[2], style->text_color[3]);
    }
    
    return clicked;
}