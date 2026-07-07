#include "zefir.h"
#include <cstdio>
#include <cmath>

int main() {
    // Init window 700x500, light background
    ZefirContext* ctx = zefir_init("Dashboard", 700, 500);
    if (!ctx) return -1;
    zefir_set_background_color(ctx, 252.0f/255.0f, 249.0f/255.0f, 238.0f/255.0f, 1.0f);
    
    // Simulated metrics
    float cpu = 0.45f, mem = 0.72f, disk = 0.33f, net = 0.18f;
    
    // Main loop
    while (!zefir_should_close(ctx)) {
        zefir_begin_frame(ctx);
        
        zefir_label(ctx, "SYSTEM DASHBOARD", 250, 10);
        
        // ---- CPU Panel ----
        zefir_panel(ctx, 15, 35, 320, 200);
        zefir_label(ctx, "CPU Usage", 120, 45);
        
        ZefirProgressStyle ps;
        zefir_get_default_progress_style(&ps);
        zefir_progress_bar(ctx, 30, 80, 290, cpu, &ps);
        cpu += 0.002f; if (cpu > 1) cpu = 0.1f;  // Animate
        
        char buf[32];
        sprintf(buf, "%.0f%%", cpu * 100);
        zefir_label(ctx, buf, 140, 120);
        
        // Mini bar chart (20 bars)
        for (int i = 0; i < 20; i++) {
            float h = 20 + (sinf(cpu * 10 + i * 0.5f) + 1) * 30;
            zefir_draw_rect(ctx, 30 + i * 14, 180 - h, 10, h, 0.2f, 0.5f, 0.9f, 1);
        }
        
        // ---- Memory Panel ----
        zefir_panel(ctx, 350, 35, 335, 200);
        zefir_label(ctx, "Memory Usage", 460, 45);
        zefir_progress_bar(ctx, 365, 80, 305, mem, &ps);
        sprintf(buf, "%.0f%%", mem * 100);
        zefir_label(ctx, buf, 500, 120);
        
        // ---- Disk Panel ----
        zefir_panel(ctx, 15, 250, 320, 200);
        zefir_label(ctx, "Disk Usage", 125, 260);
        zefir_progress_bar(ctx, 30, 295, 290, disk, &ps);
        sprintf(buf, "%.0f%%", disk * 100);
        zefir_label(ctx, buf, 140, 335);
        
        // ---- Network Panel ----
        zefir_panel(ctx, 350, 250, 335, 200);
        zefir_label(ctx, "Network", 480, 260);
        zefir_progress_bar(ctx, 365, 295, 305, net, &ps);
        sprintf(buf, "%.0f%%", net * 100);
        zefir_label(ctx, buf, 500, 335);
        
        zefir_end_frame(ctx);
    }
    
    zefir_shutdown(ctx);
    return 0;
}