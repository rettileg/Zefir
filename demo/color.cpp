#include "zefir.h"
#include <cstdio>

int main() {
    // Init window 450x350, light background
    ZefirContext* ctx = zefir_init("Color Picker", 450, 350);
    if (!ctx) return -1;
    zefir_set_background_color(ctx, 252.0f/255.0f, 249.0f/255.0f, 238.0f/255.0f, 1.0f);
    
    // RGB values (0.0 - 1.0)
    float r = 0.8f, g = 0.3f, b = 0.5f;
    
    // Main loop
    while (!zefir_should_close(ctx)) {
        zefir_begin_frame(ctx);
        
        zefir_label(ctx, "COLOR PICKER", 150, 15);
        
        // Red slider
        char buf[32];
        sprintf(buf, "R: %.0f", r * 255);
        zefir_label(ctx, buf, 30, 50);
        r = zefir_slider(ctx, 30, 70, 250, 30, r, 0, 1);
        
        // Green slider
        sprintf(buf, "G: %.0f", g * 255);
        zefir_label(ctx, buf, 30, 110);
        g = zefir_slider(ctx, 30, 130, 250, 30, g, 0, 1);
        
        // Blue slider
        sprintf(buf, "B: %.0f", b * 255);
        zefir_label(ctx, buf, 30, 170);
        b = zefir_slider(ctx, 30, 190, 250, 30, b, 0, 1);
        
        // Color preview rectangle
        zefir_label(ctx, "Preview:", 310, 50);
        zefir_draw_rect(ctx, 310, 80, 100, 100, r, g, b, 1);
        
        // Hex color code
        int ri = r * 255, gi = g * 255, bi = b * 255;
        char hex[16];
        sprintf(hex, "#%02X%02X%02X", ri, gi, bi);
        zefir_label(ctx, hex, 310, 200);
        
        // Print to console
        if (zefir_button(ctx, 310, 230, 100, 30)) {
            printf("Color: %s (%.0f, %.0f, %.0f)\n", hex, r*255, g*255, b*255);
        }
        
        zefir_end_frame(ctx);
    }
    
    zefir_shutdown(ctx);
    return 0;
}