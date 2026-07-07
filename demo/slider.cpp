#include "zefir.h"
#include <cstdio>

int main() {
    // Init window 400x300, dark background
    ZefirContext* ctx = zefir_init("Slider Control", 400, 300);
    if (!ctx) return -1;
    zefir_set_background_color(ctx, 0.2f, 0.2f, 0.2f, 1);
    
    float volume = 0.7f, brightness = 0.5f, contrast = 0.8f;
    
    // Main loop
    while (!zefir_should_close(ctx)) {
        zefir_begin_frame(ctx);
        
        // Volume
        char buf[32];
        sprintf(buf, "Volume: %.0f%%", volume * 100);
        zefir_label(ctx, buf, 50, 30);
        volume = zefir_slider(ctx, 50, 55, 300, 30, volume, 0, 1);
        
        // Brightness
        sprintf(buf, "Brightness: %.0f%%", brightness * 100);
        zefir_label(ctx, buf, 50, 100);
        brightness = zefir_slider(ctx, 50, 125, 300, 30, brightness, 0, 1);
        
        // Contrast
        sprintf(buf, "Contrast: %.0f%%", contrast * 100);
        zefir_label(ctx, buf, 50, 170);
        contrast = zefir_slider(ctx, 50, 195, 300, 30, contrast, 0, 1);
        
        zefir_end_frame(ctx);
    }
    
    zefir_shutdown(ctx);
    return 0;
}