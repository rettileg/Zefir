#include "context_menu.h"
#include "../input.h"
#include "../renderer.h"
#include "../font.h"
#include <cstring>
#include <windows.h>
#include <vector>

struct MenuItem {
    char text[64];
    bool separator;
};

struct ContextMenuState {
    bool open;
    bool first_frame;
    float x, y;
    float width;
    float item_h;
    std::vector<MenuItem> items;
    int clicked_index;
    bool was_right_down;
};

static ContextMenuState g_ctxmenu;

bool context_menu_begin(ZefirContext* ctx) {
    if (!ctx) return false;
    
    Input* input = (Input*)zefir_get_input(ctx);
    if (!input) return false;
    
    bool right_down = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    
    if (right_down && !g_ctxmenu.was_right_down) {
        float mx, my;
        input_get_mouse(input, &mx, &my);
        g_ctxmenu.open = true;
        g_ctxmenu.first_frame = true;
        g_ctxmenu.x = mx;
        g_ctxmenu.y = my;
        g_ctxmenu.width = 200;
        g_ctxmenu.item_h = 28;
        g_ctxmenu.items.clear();
        g_ctxmenu.clicked_index = -1;
    }
    g_ctxmenu.was_right_down = right_down;
    
    if (!g_ctxmenu.open) return false;
    
    return true;
}

void context_menu_end(ZefirContext* ctx) {
    if (!ctx || !g_ctxmenu.open) return;
    
    Input* input = (Input*)zefir_get_input(ctx);
    Renderer* r = (Renderer*)zefir_get_renderer(ctx);
    Font* font = (Font*)zefir_get_font(ctx);
    if (!input || !r || !font) return;
    
    int count = (int)g_ctxmenu.items.size();
    float mh = count * g_ctxmenu.item_h;
    if (mh < 1) mh = 1;
    
    bool left_click = input_mouse_released(input);
    
    float menu_bg[4] = {0.98f, 0.98f, 0.98f, 1.0f};
    float border[4] = {0.65f, 0.65f, 0.65f, 1.0f};
    float shadow[4] = {0.15f, 0.15f, 0.15f, 0.4f};
    float dummy[4] = {0,0,0,0};
    
    renderer_draw_rect_ex(r, g_ctxmenu.x + 3, g_ctxmenu.y + 3, g_ctxmenu.width, mh,
                          shadow[0], shadow[1], shadow[2], shadow[3], 0, 0, 0, dummy, dummy, dummy);

    renderer_draw_rect_ex(r, g_ctxmenu.x, g_ctxmenu.y, g_ctxmenu.width, mh,
                          menu_bg[0], menu_bg[1], menu_bg[2], menu_bg[3], 0, 0, 1.0f, border, dummy, dummy);
    
    for (int i = 0; i < count; i++) {
        float iy = g_ctxmenu.y + i * g_ctxmenu.item_h;
        bool hovered = input_is_hovering(input, g_ctxmenu.x, iy, g_ctxmenu.width, g_ctxmenu.item_h);
        
        if (g_ctxmenu.items[i].separator) {
            float sc[4] = {0.8f, 0.8f, 0.8f, 0.8f};
            renderer_draw_rect_ex(r, g_ctxmenu.x + 8, iy + g_ctxmenu.item_h/2, g_ctxmenu.width - 16, 1,
                                  sc[0], sc[1], sc[2], sc[3], 0, 0, 0, sc, dummy, dummy);
        } else {
            if (hovered) {
                float hc[4] = {0.2f, 0.45f, 0.9f, 0.95f};
                renderer_draw_rect_ex(r, g_ctxmenu.x + 1, iy, g_ctxmenu.width - 2, g_ctxmenu.item_h,
                                      hc[0], hc[1], hc[2], hc[3], 0, 0, 0, hc, dummy, dummy);
            }
            
            float tc[4] = {0.0f, 0.0f, 0.0f, 1.0f};
            if (hovered) { tc[0] = 1.0f; tc[1] = 1.0f; tc[2] = 1.0f; }
            font_render_text(font, g_ctxmenu.items[i].text, g_ctxmenu.x + 12, iy + 5, tc[0], tc[1], tc[2], tc[3]);
            
            if (hovered && left_click) {
                g_ctxmenu.clicked_index = i;
                g_ctxmenu.open = false;
            }
        }
    }
    
    if (left_click && !input_is_hovering(input, g_ctxmenu.x, g_ctxmenu.y, g_ctxmenu.width, mh)) {
        g_ctxmenu.open = false;
    }
    
    g_ctxmenu.first_frame = false;
}

bool context_menu_item(ZefirContext* ctx, const char* text) {
    if (!ctx || !g_ctxmenu.open) return false;
    
    if (g_ctxmenu.first_frame) {
        MenuItem item;
        strncpy(item.text, text, 63);
        item.text[63] = 0;
        item.separator = false;
        g_ctxmenu.items.push_back(item);
    }
    
    int idx = (int)g_ctxmenu.items.size() - 1;
    if (g_ctxmenu.clicked_index == idx) {
        g_ctxmenu.clicked_index = -1;
        return true;
    }
    
    return false;
}

void context_menu_separator(ZefirContext* ctx) {
    if (!ctx || !g_ctxmenu.open || !g_ctxmenu.first_frame) return;
    MenuItem item;
    item.separator = true;
    g_ctxmenu.items.push_back(item);
}