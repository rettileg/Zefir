#include "zefir.h"

int main() {
    // Create window 400x200
    ZefirContext* ctx = zefir_init("Hello Zefir!", 400, 200);
    if (!ctx) return -1;
    
    // Light beige background
    zefir_set_background_color(ctx, 252.0f/255.0f, 249.0f/255.0f, 238.0f/255.0f, 1.0f);
    
    // Main loop
    while (!zefir_should_close(ctx)) {
        zefir_begin_frame(ctx);
        zefir_label(ctx, "Hello Zefir!", 150, 80);
        zefir_end_frame(ctx);
    }
    
    zefir_shutdown(ctx);
    return 0;
}