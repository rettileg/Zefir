#include "zefir.h"
#include "zf_menu.h"
#include <cstdio>
#include <string>
#include <vector>

int main() {
    // Init window 800x600, dark background
    ZefirContext* ctx = zefir_init("Zefir Showcase", 800, 600);
    if (!ctx) return -1;
    zefir_set_background_color(ctx, 0.2f, 0.2f, 0.2f, 1);
    
    // Two movable windows
    ZefirGuiWindow* win  = zefir_window_create("Widgets",  50, 50, 400, 350);
    ZefirGuiWindow* win2 = zefir_window_create("Controls", 480, 50, 280, 300);
    
    float vol = 0.7f;
    bool check = true;
    int radio = 0;
    char text[256] = "Hello!";
    
    // Deletable items list
    std::vector<std::string> items_list = {"Item 1", "Item 2", "Item 3"};
    int selected_item = -1;
    
    // Button with text
    ZefirButtonStyle btn_style;
    zefir_get_default_button_style(&btn_style);
    btn_style.text = "Button";
    btn_style.font_size = 14;
    
    // Main loop
    while (!zefir_should_close(ctx)) {
        zefir_begin_frame(ctx);
        
        // ---- Menu Bar ----
        zefir_menu_bar_begin(ctx);
        zefir_menu_item(ctx, "File");
        zefir_menu_item(ctx, "Edit");
        zefir_menu_item(ctx, "Help");
        zefir_menu_bar_end(ctx);
        
        // ---- Window 1: Widgets ----
        if (zefir_window_begin(win, ctx)) {
            float wx, wy;
            zefir_window_get_pos(win, &wx, &wy);
            float ox = wx + 10, oy = wy + 35;
            
            // Button + Checkbox + Radio
            zefir_button_ex(ctx, ox, oy, 100, 30, &btn_style);
            zefir_checkbox(ctx, ox + 120, oy + 5, "Check", &check);
            zefir_radio(ctx, ox, oy + 40, "Opt A", &radio, 0);
            zefir_radio(ctx, ox + 80, oy + 40, "Opt B", &radio, 1);
            
            // Slider + Progress
            vol = zefir_slider(ctx, ox, oy + 75, 350, 25, vol, 0, 1);
            ZefirProgressStyle ps;
            zefir_get_default_progress_style(&ps);
            zefir_progress_bar(ctx, ox, oy + 110, 350, vol, &ps);
            
            // Text input
            zefir_textbox(ctx, ox, oy + 145, 350, 30, text, 256);
            
            // Selectable items
            for (int i = 0; i < (int)items_list.size(); i++) {
                float iy = oy + 190 + i * 25;
                zefir_label(ctx, items_list[i].c_str(), ox, iy);
                
                // Click to select
                if (zefir_is_hovering(ctx, ox, iy, 350, 22) && zefir_mouse_clicked(ctx))
                    selected_item = i;
                
                // Highlight selected
                if (selected_item == i)
                    zefir_draw_rect(ctx, ox - 2, iy - 2, 354, 24, 0.3f, 0.5f, 0.9f, 0.3f);
            }
            
            zefir_window_end(win, ctx);
        }
        
        // ---- Window 2: ComboBox ----
        if (zefir_window_begin(win2, ctx)) {
            float wx, wy;
            zefir_window_get_pos(win2, &wx, &wy);
            float ox = wx + 10, oy = wy + 35;
            
            const char* apis[] = {"OpenGL", "Vulkan", "DirectX", "Metal"};
            static int sel = 0;
            ZefirComboStyle cs;
            zefir_get_default_combo_style(&cs);
            zefir_combo(ctx, ox, oy, 200, apis, 4, &sel, &cs);
            
            zefir_label(ctx, "Selected API:", ox, oy + 40);
            zefir_label(ctx, apis[sel], ox, oy + 60);
            
            zefir_window_end(win2, ctx);
        }
        
        // ---- Context Menu (right-click) ----
        if (zefir_context_menu_begin(ctx)) {
            if (zefir_context_menu_item(ctx, "Delete")) {
                if (selected_item >= 0 && selected_item < (int)items_list.size()) {
                    printf("Deleted: %s\n", items_list[selected_item].c_str());
                    items_list.erase(items_list.begin() + selected_item);
                    selected_item = -1;
                }
            }
            zefir_context_menu_end(ctx);
        }
        
        zefir_end_frame(ctx);
    }
    
    zefir_window_destroy(win);
    zefir_window_destroy(win2);
    zefir_shutdown(ctx);
    return 0;
}