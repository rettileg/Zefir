#include "zefir.h"
#include <cstdio>

int main() {
    // Init window 500x450, light background
    ZefirContext* ctx = zefir_init("Settings Editor", 500, 450);
    if (!ctx) return -1;
    zefir_set_background_color(ctx, 252.0f/255.0f, 249.0f/255.0f, 238.0f/255.0f, 1.0f);
    
    // Setting struct: name, pointer to value, min, max
    struct Setting { const char* name; float* value; float min, max; };
    
    float vol = 0.7f, bright = 0.8f, contrast = 0.6f, gamma = 0.5f;
    Setting settings[] = {
        {"Volume",     &vol,      0,    1},
        {"Brightness", &bright,   0,    1},
        {"Contrast",   &contrast, 0,    1},
        {"Gamma",      &gamma,    0.1f, 2.0f},
    };
    
    bool changed = false;  // Track unsaved changes
    
    // Save button
    ZefirButtonStyle save_btn;
    zefir_get_default_button_style(&save_btn);
    save_btn.text = "Save JSON"; save_btn.font_size = 14;
    
    // Reset button
    ZefirButtonStyle reset_btn;
    zefir_get_default_button_style(&reset_btn);
    reset_btn.text = "Reset"; reset_btn.font_size = 14;
    
    // Slider style
    ZefirSliderStyle slider_style;
    zefir_get_default_slider_style(&slider_style);
    
    // Main loop
    while (!zefir_should_close(ctx)) {
        zefir_begin_frame(ctx);
        
        zefir_label(ctx, "SETTINGS EDITOR", 160, 20);
        
        // Draw 4 sliders
        for (int i = 0; i < 4; i++) {
            float y = 60 + i * 60;
            zefir_label(ctx, settings[i].name, 40, y);
            float old = *settings[i].value;
            *settings[i].value = zefir_slider_ex(ctx, 150, y + 5, 250, 30, 
                *settings[i].value, settings[i].min, settings[i].max, &slider_style);
            if (*settings[i].value != old) changed = true;
        }
        
        // Save to JSON (console output)
        if (zefir_button_ex(ctx, 100, 320, 130, 35, &save_btn)) {
            printf("{\n");
            for (int i = 0; i < 4; i++)
                printf("  \"%s\": %.2f%s\n", settings[i].name, *settings[i].value, i<3 ? "," : "");
            printf("}\n");
            changed = false;
        }
        
        // Reset to defaults
        if (zefir_button_ex(ctx, 260, 320, 120, 35, &reset_btn)) {
            vol = 0.5f; bright = 0.5f; contrast = 0.5f; gamma = 1.0f;
            changed = true;
        }
        
        // Unsaved indicator
        if (changed) zefir_label(ctx, "* Unsaved changes", 150, 380);
        
        zefir_end_frame(ctx);
    }
    
    zefir_shutdown(ctx);
    return 0;
}