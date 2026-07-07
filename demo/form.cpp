#include "zefir.h"
#include <cstdio>

int main() {
    // Init window 400x350, light background
    ZefirContext* ctx = zefir_init("Simple Form", 400, 350);
    if (!ctx) return -1;
    zefir_set_background_color(ctx, 252.0f/255.0f, 249.0f/255.0f, 238.0f/255.0f, 1.0f);
    
    // Input buffers
    char name[256] = "";
    char email[256] = "";
    
    // Submit button with text
    ZefirButtonStyle btn;
    zefir_get_default_button_style(&btn);
    btn.text = "Submit";
    btn.font_size = 15;
    
    // Main loop
    while (!zefir_should_close(ctx)) {
        zefir_begin_frame(ctx);
        
        // Name field
        zefir_label(ctx, "Name:", 50, 40);
        zefir_textbox(ctx, 50, 65, 300, 35, name, 256);
        
        // Email field
        zefir_label(ctx, "Email:", 50, 115);
        zefir_textbox(ctx, 50, 140, 300, 35, email, 256);
        
        // Submit button
        if (zefir_button_ex(ctx, 150, 200, 100, 40, &btn)) {
            printf("Submitted!\nName: %s\nEmail: %s\n", name, email);
        }
        
        zefir_end_frame(ctx);
    }
    
    zefir_shutdown(ctx);
    return 0;
}