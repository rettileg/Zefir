#include "zefir.h"
#include "zf_theme.h"
#include <cstdio>

int main() {
    // Init window 500x450
    ZefirContext* ctx = zefir_init("Theme Switcher", 500, 450);
    if (!ctx) return -1;
    
    // Load 5 themes
    ZefirTheme themes[5];
    zefir_theme_get_light(&themes[0]);
    zefir_theme_get_dark(&themes[1]);
    zefir_theme_get_ocean(&themes[2]);
    zefir_theme_get_forest(&themes[3]);
    zefir_theme_get_sunset(&themes[4]);
    
    const char* theme_names[] = {"Light", "Dark", "Ocean", "Forest", "Sunset"};
    int current = 0;
    
    float vol = 0.7f;
    bool opt = true;
    char text[256] = "Zefir GUI";
    
    // Theme selection buttons
    ZefirButtonStyle theme_btn;
    zefir_get_default_button_style(&theme_btn);
    theme_btn.font_size = 12;
    theme_btn.corner_radius = 4;
    
    // Demo button
    ZefirButtonStyle demo_btn;
    zefir_get_default_button_style(&demo_btn);
    demo_btn.text = "Button";
    demo_btn.font_size = 14;
    
    // Main loop
    while (!zefir_should_close(ctx)) {
        zefir_begin_frame(ctx);
        
        // Apply current theme background
        ZefirTheme* t = &themes[current];
        zefir_set_background_color(ctx, t->bg_color[0], t->bg_color[1], t->bg_color[2], 1);
        
        zefir_label(ctx, "THEME SWITCHER", 170, 20);
        
        // Theme buttons
        for (int i = 0; i < 5; i++) {
            theme_btn.text = theme_names[i];
            if (zefir_button_ex(ctx, 30 + i * 90, 55, 80, 30, &theme_btn)) {
                current = i;
                printf("Theme: %s\n", theme_names[i]);
            }
        }
        
        zefir_label(ctx, theme_names[current], 200, 100);
        
        // Widgets preview panel
        zefir_panel(ctx, 40, 130, 420, 280);
        zefir_button_ex(ctx, 60, 150, 100, 35, &demo_btn);
        vol = zefir_slider(ctx, 180, 155, 200, 30, vol, 0, 1);
        zefir_checkbox(ctx, 60, 210, "Enable feature", &opt);
        zefir_textbox(ctx, 60, 250, 300, 35, text, 256);
        
        zefir_end_frame(ctx);
    }
    
    zefir_shutdown(ctx);
    return 0;
}