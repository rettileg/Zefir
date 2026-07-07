#include "zefir.h"
#include "layout.h"
#include <cstdio>

int main() {
    // Init window 600x450, light background
    ZefirContext* ctx = zefir_init("Layout Demo", 600, 450);
    if (!ctx) return -1;
    zefir_set_background_color(ctx, 252.0f/255.0f, 249.0f/255.0f, 238.0f/255.0f, 1.0f);
    
    // Left column (VBox)
    ZefirLayout* vbox = zefir_layout_create(20, 30, 250, 400, 1);  // 1 = VBox
    
    // Right column (VBox)
    ZefirLayout* hbox = zefir_layout_create(290, 30, 290, 400, 1); // 1 = VBox
    
    float vol = 0.7f, speed = 0.5f;
    bool opt1 = false, opt2 = true, opt3 = true;
    char name[256] = "Zefir";
    int quality = 1;
    
    // Main loop
    while (!zefir_should_close(ctx)) {
        zefir_begin_frame(ctx);
        
        // ---- Left Column ----
        zefir_label(ctx, "Controls", 25, 15);
        zefir_layout_begin(vbox);
        float x, y, w, h;
        if (zefir_layout_next(vbox, &x, &y, &w, &h, 35)) zefir_button(ctx, x, y, w, h);
        if (zefir_layout_next(vbox, &x, &y, &w, &h, 35)) zefir_button(ctx, x, y, w, h);
        if (zefir_layout_next(vbox, &x, &y, &w, &h, 35)) vol = zefir_slider(ctx, x, y, w, h, vol, 0, 1);
        if (zefir_layout_next(vbox, &x, &y, &w, &h, 25)) zefir_checkbox(ctx, x, y, "Option A", &opt1);
        if (zefir_layout_next(vbox, &x, &y, &w, &h, 25)) zefir_checkbox(ctx, x, y, "Option B", &opt2);
        if (zefir_layout_next(vbox, &x, &y, &w, &h, 25)) zefir_checkbox(ctx, x, y, "Option C", &opt3);
        if (zefir_layout_next(vbox, &x, &y, &w, &h, 35)) zefir_textbox(ctx, x, y, w, h, name, 256);
        zefir_layout_end(vbox);
        
        // ---- Right Column ----
        zefir_label(ctx, "Settings", 295, 15);
        zefir_layout_begin(hbox);
        if (zefir_layout_next(hbox, &x, &y, &w, &h, 35)) zefir_radio(ctx, x, y, "Low", &quality, 0);
        if (zefir_layout_next(hbox, &x, &y, &w, &h, 35)) zefir_radio(ctx, x, y, "Medium", &quality, 1);
        if (zefir_layout_next(hbox, &x, &y, &w, &h, 35)) zefir_radio(ctx, x, y, "High", &quality, 2);
        if (zefir_layout_next(hbox, &x, &y, &w, &h, 35)) speed = zefir_slider(ctx, x, y, w, h, speed, 0, 1);
        
        ZefirProgressStyle ps;
        zefir_get_default_progress_style(&ps);
        if (zefir_layout_next(hbox, &x, &y, &w, &h, 25)) zefir_progress_bar(ctx, x, y, w, speed, &ps);
        zefir_layout_end(hbox);
        
        zefir_end_frame(ctx);
    }
    
    zefir_layout_destroy(vbox);
    zefir_layout_destroy(hbox);
    zefir_shutdown(ctx);
    return 0;
}