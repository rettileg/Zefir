#include "zefir.h"
#include <cstdio>
#include <vector>
#include <string>
#include <cstring>

int main() {
    // Init window 450x500, light background
    ZefirContext* ctx = zefir_init("To-Do List", 450, 500);
    if (!ctx) return -1;
    zefir_set_background_color(ctx, 252.0f/255.0f, 249.0f/255.0f, 238.0f/255.0f, 1);
    
    char input[256] = "";
    std::vector<std::string> tasks;
    bool completed[100] = {};
    float scroll = 0;
    
    ZefirScrollBarStyle sb_style;
    zefir_get_default_scrollbar_style(&sb_style);
    
    // Add button (green text)
    ZefirButtonStyle add_style;
    zefir_get_default_button_style(&add_style);
    add_style.text = "Add";
    add_style.font_size = 15;
    
    // Delete button (red X)
    ZefirButtonStyle del_style;
    zefir_get_default_button_style(&del_style);
    del_style.text = "X";
    del_style.font_size = 14;
    del_style.text_color[0] = 0.8f; del_style.text_color[1] = 0.2f; del_style.text_color[2] = 0.2f;
    
    // Main loop
    while (!zefir_should_close(ctx)) {
        zefir_begin_frame(ctx);
        
        zefir_label(ctx, "TO-DO LIST", 160, 20);
        
        // Input + Add button
        zefir_textbox(ctx, 30, 50, 280, 35, input, 256);
        if (zefir_button_ex(ctx, 320, 50, 80, 35, &add_style) && input[0]) {
            if (tasks.size() < 100) {
                tasks.push_back(input);
                input[0] = 0;
            }
        }
        
        // Task list
        float list_y = 100;
        for (int i = 0; i < (int)tasks.size(); i++) {
            float iy = list_y + i * 35 - scroll;
            if (iy > 95 && iy < 450) {
                char buf[256];
                strcpy(buf, tasks[i].c_str());
                zefir_checkbox(ctx, 30, iy, buf, &completed[i]);
                if (zefir_button_ex(ctx, 360, iy, 30, 25, &del_style)) {
                    tasks.erase(tasks.begin() + i);
                    break;
                }
            }
        }
        
        // Scrollbar
        int total_h = tasks.size() * 35;
        if (total_h > 350)
            scroll = zefir_scrollbar_v(ctx, 420, 100, 12, 350, &scroll, total_h, 350, &sb_style);
        
        zefir_end_frame(ctx);
    }
    
    zefir_shutdown(ctx);
    return 0;
}