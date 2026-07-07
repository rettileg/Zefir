#include "zefir.h"
#include <cstdio>

int main() {
    // Init window 400x400, dark background
    ZefirContext* ctx = zefir_init("Volume Mixer", 400, 400);
    if (!ctx) return -1;
    zefir_set_background_color(ctx, 0.15f, 0.15f, 0.15f, 1);
    
    // Channel volumes
    float master = 0.8f, music = 0.6f, sfx = 0.9f, voice = 0.5f;
    
    // White label style
    ZefirLabelStyle label_style;
    zefir_get_default_label_style(&label_style);
    label_style.color[0] = 0.9f; label_style.color[1] = 0.9f; label_style.color[2] = 0.9f;
    label_style.font_size = 15;
    
    // Reset button
    ZefirButtonStyle btn_style;
    zefir_get_default_button_style(&btn_style);
    btn_style.text = "Reset";
    btn_style.font_size = 14;
    
    // Main loop
    while (!zefir_should_close(ctx)) {
        zefir_begin_frame(ctx);
        
        zefir_label_ex(ctx, "VOLUME MIXER", 130, 20, &label_style);
        
        // Master
        char buf[32];
        sprintf(buf, "Master: %.0f%%", master * 100);
        zefir_label_ex(ctx, buf, 50, 60, &label_style);
        master = zefir_slider(ctx, 50, 80, 300, 35, master, 0, 1);
        
        // Music
        sprintf(buf, "Music: %.0f%%", music * 100);
        zefir_label_ex(ctx, buf, 50, 130, &label_style);
        music = zefir_slider(ctx, 50, 150, 300, 35, music, 0, 1);
        
        // SFX
        sprintf(buf, "SFX: %.0f%%", sfx * 100);
        zefir_label_ex(ctx, buf, 50, 200, &label_style);
        sfx = zefir_slider(ctx, 50, 220, 300, 35, sfx, 0, 1);
        
        // Voice
        sprintf(buf, "Voice: %.0f%%", voice * 100);
        zefir_label_ex(ctx, buf, 50, 270, &label_style);
        voice = zefir_slider(ctx, 50, 290, 300, 35, voice, 0, 1);
        
        // Reset all to 50%
        if (zefir_button_ex(ctx, 150, 340, 100, 35, &btn_style)) {
            master = music = sfx = voice = 0.5f;
            printf("Reset\n");
        }
        
        zefir_end_frame(ctx);
    }
    
    zefir_shutdown(ctx);
    return 0;
}