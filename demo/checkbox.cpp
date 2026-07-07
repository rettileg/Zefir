#include "zefir.h"
#include <cstdio>

int main() {
    // Init window 400x350, dark background
    ZefirContext* ctx = zefir_init("CheckBoxes", 400, 350);
    if (!ctx) return -1;
    zefir_set_background_color(ctx, 0.2f, 0.2f, 0.2f, 1);
    
    // Toggle states
    bool option1 = false;
    bool option2 = true;
    bool option3 = false;
    bool option4 = true;
    
    // White label style (for dark background)
    ZefirLabelStyle ls;
    zefir_get_default_label_style(&ls);
    ls.color[0] = 0.9f; ls.color[1] = 0.9f; ls.color[2] = 0.9f;
    
    // Checkbox style with white text
    ZefirCheckBoxStyle cb_style;
    zefir_get_default_checkbox_style(&cb_style);
    cb_style.text_color[0] = 0.9f;
    cb_style.text_color[1] = 0.9f;
    cb_style.text_color[2] = 0.9f;
    
    // Main loop
    while (!zefir_should_close(ctx)) {
        zefir_begin_frame(ctx);
        
        // Title
        zefir_label_ex(ctx, "Settings:", 50, 40, &ls);
        
        // Checkboxes with labels
        zefir_checkbox_ex(ctx, 50, 75,  0, "Enable sound", &option1, &cb_style);
        zefir_checkbox_ex(ctx, 50, 105, 0, "Fullscreen",   &option2, &cb_style);
        zefir_checkbox_ex(ctx, 50, 135, 0, "Show FPS",     &option3, &cb_style);
        zefir_checkbox_ex(ctx, 50, 165, 0, "VSync",        &option4, &cb_style);
        
        // Status line
        char status[128];
        sprintf(status, "Sound:%d Full:%d FPS:%d VSync:%d", option1, option2, option3, option4);
        zefir_label_ex(ctx, status, 50, 220, &ls);
        
        zefir_end_frame(ctx);
    }
    
    zefir_shutdown(ctx);
    return 0;
}