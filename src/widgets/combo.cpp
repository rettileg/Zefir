#include "combo.h"
#include "../input.h"
#include "../renderer.h"
#include "../font.h"
#include <vector>

struct ComboData {
    float x, y, w;
    const char** items;
    int item_count;
    int* selected;
    ZefirComboStyle style;
    bool open;
    int hovered;
};

static std::vector<ComboData> g_combos;

void combo_get_default_style(ZefirComboStyle* s) {
    if (!s) return;
    s->item_height = 28.0f;
    s->font_size = 15.0f;
    s->text_color[0] = 0.1f; s->text_color[1] = 0.1f; s->text_color[2] = 0.1f; s->text_color[3] = 1;
    s->bg_color[0] = 0.97f; s->bg_color[1] = 0.97f; s->bg_color[2] = 0.97f; s->bg_color[3] = 1;
    s->hover_color[0] = 0.2f; s->hover_color[1] = 0.45f; s->hover_color[2] = 0.9f; s->hover_color[3] = 0.3f;
    s->border_color[0] = 0.5f; s->border_color[1] = 0.5f; s->border_color[2] = 0.5f; s->border_color[3] = 1;
    s->corner_radius = 4.0f;
    s->max_visible_items = 5;
}

int combo_draw(ZefirContext* ctx, float x, float y, float w, const char** items, int item_count, int* selected, const ZefirComboStyle* style) {
    if (!ctx || !items || !selected || !style) return *selected;
    
    Input* input = (Input*)zefir_get_input(ctx);
    Renderer* r = (Renderer*)zefir_get_renderer(ctx);
    Font* font = (Font*)zefir_get_font(ctx);
    const float* bg = zefir_get_bg_color(ctx);
    if (!input || !r || !font || !bg) return *selected;
    
    float ih = style->item_height;
    float dummy[4] = {0,0,0,0};
    float highlight[4] = {1,1,1,0.3f};
    float shadow[4] = {0.3f,0.3f,0.3f,0.3f};
    
    ComboData* cd = nullptr;
    for (auto& c : g_combos) {
        if (c.selected == selected) { cd = &c; break; }
    }
    if (!cd) {
        ComboData new_cd;
        new_cd.x = x; new_cd.y = y; new_cd.w = w;
        new_cd.items = items; new_cd.item_count = item_count;
        new_cd.selected = selected; new_cd.style = *style;
        new_cd.open = false; new_cd.hovered = -1;
        g_combos.push_back(new_cd);
        cd = &g_combos.back();
    }
    
    cd->x = x; cd->y = y; cd->w = w;
    
    bool hover_main = input_is_hovering(input, x, y, w, ih);
    if (hover_main && input_mouse_released(input)) {
        cd->open = !cd->open;
    }
    
    float main_bg[4] = {bg[0]*0.95f, bg[1]*0.95f, bg[2]*0.95f, 1};
    renderer_draw_rect_ex(r, x, y, w, ih, main_bg[0], main_bg[1], main_bg[2], main_bg[3],
                          style->corner_radius, 1, 1.5f, style->border_color, highlight, shadow);
    
    font_render_text(font, "▼", x + w - 25, y + 4, 0.3f, 0.3f, 0.3f, 1);
    
    if (*selected >= 0 && *selected < item_count) {
        font_render_text(font, items[*selected], x + 8, y + 5,
                        style->text_color[0], style->text_color[1], style->text_color[2], style->text_color[3]);
    }
    
    return *selected;
}

void combo_render_all(ZefirContext* ctx) {
    if (!ctx) return;
    
    Input* input = (Input*)zefir_get_input(ctx);
    Renderer* r = (Renderer*)zefir_get_renderer(ctx);
    Font* font = (Font*)zefir_get_font(ctx);
    if (!input || !r || !font) return;
    
    float dummy[4] = {0,0,0,0};
    float shadow[4] = {0.2f,0.2f,0.2f,0.5f};
    
    for (auto& cd : g_combos) {
        if (!cd.open) continue;
        
        float ih = cd.style.item_height;
        int visible = cd.item_count;
        if (visible > cd.style.max_visible_items) visible = cd.style.max_visible_items;
        float list_h = visible * ih;
        
        float list_bg[4] = {0.99f, 0.99f, 0.99f, 1};
        renderer_draw_rect_ex(r, cd.x, cd.y + ih, cd.w, list_h,
                              list_bg[0], list_bg[1], list_bg[2], list_bg[3],
                              0, 0, 1.0f, cd.style.border_color, dummy, shadow);
        
        for (int i = 0; i < visible; i++) {
            float iy = cd.y + ih + i * ih;
            bool hov = input_is_hovering(input, cd.x, iy, cd.w, ih);
            
            if (hov) {
                float hc[4] = {cd.style.hover_color[0], cd.style.hover_color[1], cd.style.hover_color[2], 0.8f};
                renderer_draw_rect_ex(r, cd.x + 1, iy, cd.w - 2, ih, hc[0], hc[1], hc[2], hc[3], 0, 0, 0, hc, dummy, dummy);
            }
            
            float tc[4] = {cd.style.text_color[0], cd.style.text_color[1], cd.style.text_color[2], cd.style.text_color[3]};
            if (hov) { tc[0] = 1; tc[1] = 1; tc[2] = 1; }
            if (i == *cd.selected) { tc[0] = cd.style.hover_color[0]; tc[1] = cd.style.hover_color[1]; tc[2] = cd.style.hover_color[2]; }
            
            font_render_text(font, cd.items[i], cd.x + 8, iy + 4, tc[0], tc[1], tc[2], tc[3]);
            
            if (hov && input_mouse_released(input)) {
                *cd.selected = i;
                cd.open = false;
            }
        }
        
        if (input_mouse_released(input) && 
            !input_is_hovering(input, cd.x, cd.y, cd.w, ih) &&
            !input_is_hovering(input, cd.x, cd.y + ih, cd.w, list_h)) {
            cd.open = false;
        }
    }
}