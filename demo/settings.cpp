#include "zefir.h"
#include <cstdio>

int main() {
    // Init window 500x400, light background
    ZefirContext* ctx = zefir_init("Settings", 500, 400);
    if (!ctx) return -1;
    zefir_set_background_color(ctx, 252.0f/255.0f, 249.0f/255.0f, 238.0f/255.0f, 1);
    
    // Tab names
    const char* tabs[] = {"General", "Graphics", "Audio", "Controls"};
    int active = 0;
    ZefirTabViewStyle tab_style;
    zefir_get_default_tabview_style(&tab_style);
    
    // Settings variables
    bool fullscreen = false, vsync = true;
    float volume = 0.7f, sens = 0.5f;
    int quality = 1;
    char name[256] = "Player";
    
    // Main loop
    while (!zefir_should_close(ctx)) {
        zefir_begin_frame(ctx);
        
        zefir_tabview_begin(ctx, 20, 30, 460, 350, tabs, 4, &active, &tab_style);
        
        switch (active) {
            case 0: // General tab
                zefir_label(ctx, "Player Name:", 40, 80);
                zefir_textbox(ctx, 40, 100, 250, 35, name, 256);
                zefir_checkbox(ctx, 40, 150, "Fullscreen", &fullscreen);
                break;
                
            case 1: // Graphics tab
                zefir_label(ctx, "Quality:", 40, 80);
                zefir_radio(ctx, 40, 110, "Low",    &quality, 0);
                zefir_radio(ctx, 40, 140, "Medium", &quality, 1);
                zefir_radio(ctx, 40, 170, "High",   &quality, 2);
                zefir_checkbox(ctx, 40, 210, "VSync", &vsync);
                break;
                
            case 2: // Audio tab
                zefir_label(ctx, "Volume:", 40, 80);
                volume = zefir_slider(ctx, 40, 110, 300, 30, volume, 0, 1);
                break;
                
            case 3: // Controls tab
                zefir_label(ctx, "Mouse Sensitivity:", 40, 80);
                sens = zefir_slider(ctx, 40, 110, 300, 30, sens, 0, 1);
                break;
        }
        
        zefir_end_frame(ctx);
    }
    
    zefir_shutdown(ctx);
    return 0;
}