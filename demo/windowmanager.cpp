#include "zefir.h"
#include <cstdio>

int main() {
    // Init window 800x600, dark background
    ZefirContext* ctx = zefir_init("Window Manager", 800, 600);
    if (!ctx) return -1;
    zefir_set_background_color(ctx, 0.15f, 0.15f, 0.15f, 1);
    
    // 4 movable windows
    ZefirGuiWindow* wins[4];
    wins[0] = zefir_window_create("File Explorer", 50, 50, 300, 250);
    wins[1] = zefir_window_create("Settings",     400, 100, 280, 200);
    wins[2] = zefir_window_create("Console",      100, 350, 350, 200);
    wins[3] = zefir_window_create("Preview",      500, 350, 250, 200);
    
    float volume = 0.7f;
    bool checked = true;
    
    // Main loop
    while (!zefir_should_close(ctx)) {
        zefir_begin_frame(ctx);
        
        for (int i = 0; i < 4; i++) {
            if (zefir_window_begin(wins[i], ctx)) {
                float wx, wy;
                zefir_window_get_pos(wins[i], &wx, &wy);
                float ox = wx + 10, oy = wy + 35;
                
                switch (i) {
                    case 0: // File Explorer
                        zefir_label(ctx, "[DIR] Documents", ox, oy);
                        zefir_label(ctx, "[DIR] Projects",  ox, oy + 25);
                        zefir_label(ctx, "[DIR] Downloads", ox, oy + 50);
                        zefir_checkbox(ctx, ox, oy + 80, "Show hidden files", &checked);
                        break;
                    case 1: // Settings
                        zefir_label(ctx, "Volume Control:", ox, oy);
                        volume = zefir_slider(ctx, ox, oy + 25, 240, 30, volume, 0, 1);
                        break;
                    case 2: // Console
                        zefir_label(ctx, "> Build started...", ox, oy);
                        zefir_label(ctx, "> Compiling...",    ox, oy + 25);
                        zefir_label(ctx, "> Done!",           ox, oy + 50);
                        break;
                    case 3: // Preview
                        zefir_label(ctx, "Volume Level:", ox, oy);
                        ZefirProgressStyle ps;
                        zefir_get_default_progress_style(&ps);
                        zefir_progress_bar(ctx, ox, oy + 30, 200, volume, &ps);
                        break;
                }
                zefir_window_end(wins[i], ctx);
            }
        }
        
        zefir_end_frame(ctx);
    }
    
    for (int i = 0; i < 4; i++) zefir_window_destroy(wins[i]);
    zefir_shutdown(ctx);
    return 0;
}