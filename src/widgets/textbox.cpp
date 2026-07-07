#include "textbox.h"
#include "../input.h"
#include "../renderer.h"
#include "../font.h"
#include <cmath>
#include <cstring>
#include <map>

struct TextBoxState {
    bool focused;
    double cursor_blink;
    bool cursor_visible;
};

static std::map<char*, TextBoxState> g_states;

static TextBoxState& get_state(char* buffer) {
    auto it = g_states.find(buffer);
    if (it == g_states.end()) {
        TextBoxState s = {false, 0.0, true};
        g_states[buffer] = s;
        return g_states[buffer];
    }
    return it->second;
}

void textbox_get_default_style(ZefirTextBoxStyle* style) {
    if (!style) return;
    style->border_width = 2.0f;
    style->border_color[0] = 0.55f; style->border_color[1] = 0.52f;
    style->border_color[2] = 0.48f; style->border_color[3] = 1.0f;
    style->highlight_color[0] = 1.0f; style->highlight_color[1] = 1.0f;
    style->highlight_color[2] = 1.0f; style->highlight_color[3] = 0.5f;
    style->shadow_color[0] = 0.45f; style->shadow_color[1] = 0.42f;
    style->shadow_color[2] = 0.38f; style->shadow_color[3] = 0.7f;
    style->corner_radius = 3.0f;
    style->font_size = 16.0f;
    style->text_color[0] = 0.1f; style->text_color[1] = 0.1f;
    style->text_color[2] = 0.1f; style->text_color[3] = 1.0f;
    style->cursor_color[0] = 0.2f; style->cursor_color[1] = 0.2f;
    style->cursor_color[2] = 0.2f; style->cursor_color[3] = 1.0f;
    style->cursor_width = 1.5f;
    style->max_length = 256;
}

bool textbox_draw(ZefirContext* ctx, float x, float y, float w, float h,
                  char* buffer, int buffer_size, const ZefirTextBoxStyle* style) {
    if (!ctx || !buffer || !style) return false;

    Input* input = (Input*)zefir_get_input(ctx);
    Renderer* renderer = (Renderer*)zefir_get_renderer(ctx);
    Font* font = (Font*)zefir_get_font(ctx);
    const float* bg_color = zefir_get_bg_color(ctx);
    if (!input || !renderer || !bg_color) return false;

    TextBoxState& state = get_state(buffer);

    bool hovered = input_is_hovering(input, x, y, w, h);
    bool clicked = hovered && input_mouse_released(input);
    if (clicked) state.focused = true;
    if (input_mouse_pressed(input) && !hovered) state.focused = false;

    if (state.focused) {
        state.cursor_blink += 0.016;
        if (state.cursor_blink > 0.5) {
            state.cursor_blink = 0;
            state.cursor_visible = !state.cursor_visible;
        }
    }

    float bg_r = bg_color[0] * (state.focused ? 1.0f : 0.98f);
    float bg_g = bg_color[1] * (state.focused ? 1.0f : 0.98f);
    float bg_b = bg_color[2] * (state.focused ? 1.0f : 0.98f);

    renderer_draw_rect_ex(renderer, x, y, w, h, bg_r, bg_g, bg_b, 1.0f,
                          style->corner_radius, 2, style->border_width,
                          style->border_color, style->highlight_color, style->shadow_color);

    if (font) {
        float text_y = y + (h - font_get_height(font)) / 2.0f;
        font_render_text(font, buffer, x + 8, text_y,
                        style->text_color[0], style->text_color[1],
                        style->text_color[2], style->text_color[3]);
        if (state.focused && state.cursor_visible) {
            float text_w = font_get_text_width(font, buffer);
            float cursor_x = x + 8 + text_w;
            float cursor_h = font_get_height(font);
            renderer_draw_rect_ex(renderer, cursor_x, text_y,
                                  style->cursor_width, cursor_h,
                                  style->cursor_color[0], style->cursor_color[1],
                                  style->cursor_color[2], style->cursor_color[3],
                                  0, 0, 0, style->cursor_color,
                                  style->highlight_color, style->shadow_color);
        }
    }

    if (state.focused) {
        while (input_has_chars(input)) {
            unsigned int cp = input_pop_char(input);
            int len = strlen(buffer);
                if (cp == '\b' || cp == 0x7F) {
                    if (len > 0) {
                        int pos = len - 1;
                        while (pos > 0 && (buffer[pos] & 0xC0) == 0x80) pos--;
                        int char_len = len - pos;
                        if (len >= char_len) buffer[pos] = '\0';
                    }
                } else if (cp >= 32) {
                char utf8[5]; int char_len = 0;
                if (cp < 0x80) { utf8[0] = (char)cp; utf8[1] = '\0'; char_len = 1; }
                else if (cp < 0x800) { utf8[0] = (char)(0xC0 | (cp >> 6)); utf8[1] = (char)(0x80 | (cp & 0x3F)); utf8[2] = '\0'; char_len = 2; }
                else if (cp < 0x10000) { utf8[0] = (char)(0xE0 | (cp >> 12)); utf8[1] = (char)(0x80 | ((cp >> 6) & 0x3F)); utf8[2] = (char)(0x80 | (cp & 0x3F)); utf8[3] = '\0'; char_len = 3; }
                else { utf8[0] = (char)(0xF0 | (cp >> 18)); utf8[1] = (char)(0x80 | ((cp >> 12) & 0x3F)); utf8[2] = (char)(0x80 | ((cp >> 6) & 0x3F)); utf8[3] = (char)(0x80 | (cp & 0x3F)); utf8[4] = '\0'; char_len = 4; }
                if (len + char_len < buffer_size) { strcat(buffer, utf8); }
            }
            state.cursor_visible = true;
            state.cursor_blink = 0;
        }
    }

    return state.focused;
}