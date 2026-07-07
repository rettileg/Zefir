#include "checkbox.h"
#include "../input.h"
#include "../renderer.h"
#include "../font.h"
#include "../assets/checkbox_icon.h"
#include <glad/glad.h>
#include "../thirdparty/stb/stb_image.h"

static unsigned int g_check_texture = 0;
static bool g_check_tex_loaded = false;

static void load_check_texture() {
    if (g_check_tex_loaded) return;
    
    int w, h, ch;
    unsigned char* data = stbi_load_from_memory(check_icon_data, check_icon_size, &w, &h, &ch, 4);
    if (!data) return;
    
    glGenTextures(1, &g_check_texture);
    glBindTexture(GL_TEXTURE_2D, g_check_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    stbi_image_free(data);
    g_check_tex_loaded = true;
}

void checkbox_get_default_style(ZefirCheckBoxStyle* style) {
    if (!style) return;
    style->box_size = 18.0f;
    style->check_color[0] = 0.2f; style->check_color[1] = 0.6f;
    style->check_color[2] = 1.0f; style->check_color[3] = 1.0f;
    style->border_width = 1.5f;
    style->border_color[0] = 0.45f; style->border_color[1] = 0.45f;
    style->border_color[2] = 0.45f; style->border_color[3] = 1.0f;
    style->corner_radius = 3.0f;
    style->text = nullptr;
    style->font_size = 16.0f;
    style->text_color[0] = 0.15f; style->text_color[1] = 0.15f;
    style->text_color[2] = 0.15f; style->text_color[3] = 1.0f;
}

bool checkbox_draw(ZefirContext* ctx, float x, float y, float size, bool* checked, const ZefirCheckBoxStyle* style) {
    if (!ctx || !checked || !style) return *checked;
    
    Input* input = (Input*)zefir_get_input(ctx);
    Renderer* renderer = (Renderer*)zefir_get_renderer(ctx);
    Font* font = (Font*)zefir_get_font(ctx);
    const float* bg = zefir_get_bg_color(ctx);
    if (!input || !renderer || !bg) return *checked;
    
    float s = size > 0 ? size : style->box_size;
    bool hovered = input_is_hovering(input, x, y, s, s);
    bool clicked = hovered && input_mouse_released(input);
    if (clicked) *checked = !(*checked);
    
    float highlight[4] = {1,1,1,0.3f};
    float shadow[4] = {0.3f,0.3f,0.3f,0.4f};
    
    renderer_draw_rect_ex(renderer, x, y, s, s,
                          bg[0], bg[1], bg[2], 1.0f,
                          style->corner_radius, (*checked) ? 2 : 1,
                          style->border_width, style->border_color, highlight, shadow);
    
    if (*checked) {
        load_check_texture();
        float pad = s * 0.15f;
        zefir_texture_rect(ctx, g_check_texture, x + pad, y + pad, s - pad*2, s - pad*2);
    }
    
    if (style->text && font) {
        font_render_text(font, style->text, x + s + 6, y + (s - font_get_height(font)) / 2.0f,
                        style->text_color[0], style->text_color[1], style->text_color[2], style->text_color[3]);
    }
    
    return *checked;
}