#include "zefir.h"
#include <cstdio>

int main() {
    // Init window 500x300, dark background
    ZefirContext* ctx = zefir_init("Media Player", 500, 300);
    if (!ctx) return -1;
    zefir_set_background_color(ctx, 0.12f, 0.12f, 0.12f, 1);
    
    float progress = 0.35f;
    float volume = 0.7f;
    bool playing = false;
    char song[] = "My Favorite Song.mp3";
    int current_time = 85, total_time = 242;
    
    // White text for dark background
    ZefirLabelStyle ls;
    zefir_get_default_label_style(&ls);
    ls.color[0] = 0.9f; ls.color[1] = 0.9f; ls.color[2] = 0.9f;
    
    // Transport buttons
    ZefirButtonStyle prev_btn, play_btn, next_btn, stop_btn;
    
    zefir_get_default_button_style(&prev_btn);
    prev_btn.text = "|<"; prev_btn.font_size = 16;
    
    zefir_get_default_button_style(&play_btn);
    play_btn.text = ">"; play_btn.font_size = 20;
    
    zefir_get_default_button_style(&next_btn);
    next_btn.text = ">|"; next_btn.font_size = 16;
    
    zefir_get_default_button_style(&stop_btn);
    stop_btn.text = "[]"; stop_btn.font_size = 16;
    
    // Main loop
    while (!zefir_should_close(ctx)) {
        zefir_begin_frame(ctx);
        
        // Song title
        zefir_label_ex(ctx, song, 150, 20, &ls);
        
        // Progress bar
        ZefirProgressStyle ps;
        zefir_get_default_progress_style(&ps);
        zefir_progress_bar(ctx, 50, 60, 400, progress, &ps);
        
        // Time display
        char time_buf[32];
        sprintf(time_buf, "%d:%02d / %d:%02d", current_time/60, current_time%60, total_time/60, total_time%60);
        zefir_label_ex(ctx, time_buf, 190, 90, &ls);
        
        // Previous
        if (zefir_button_ex(ctx, 140, 120, 50, 50, &prev_btn)) printf("Prev\n");
        
        // Play/Pause (toggles icon)
        play_btn.text = playing ? "||" : ">";
        if (zefir_button_ex(ctx, 200, 120, 60, 50, &play_btn)) {
            playing = !playing;
        }
        
        // Next
        if (zefir_button_ex(ctx, 270, 120, 50, 50, &next_btn)) printf("Next\n");
        
        // Stop (resets progress)
        if (zefir_button_ex(ctx, 340, 120, 50, 50, &stop_btn)) {
            playing = false; progress = 0; current_time = 0;
        }
        
        // Volume slider
        zefir_label_ex(ctx, "Volume:", 50, 200, &ls);
        volume = zefir_slider(ctx, 120, 200, 200, 30, volume, 0, 1);
        
        // Simulate playback
        if (playing) {
            progress += 0.003f;
            if (progress > 1.0f) progress = 0;
            current_time = (int)(progress * total_time);
        }
        
        zefir_end_frame(ctx);
    }
    
    zefir_shutdown(ctx);
    return 0;
}