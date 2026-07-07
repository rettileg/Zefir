#include "zefir.h"
#include <cstdio>

int main() {
    // Init window 400x300
    ZefirContext* ctx = zefir_init("Button Click", 400, 300);
    if (!ctx) return -1;
    
    // Light beige background (252, 249, 238)
    zefir_set_background_color(ctx, 252.0f/255.0f, 249.0f/255.0f, 238.0f/255.0f, 1.0f);
    
    int clicks = 0;
    char text[64] = "Clicks: 0";
    
    // Button 1: large, with text
    ZefirButtonStyle btn1;
    zefir_get_default_button_style(&btn1);
    btn1.text = "Click Me!";
    btn1.font_size = 16;
    
    // Button 2: rounded corners
    ZefirButtonStyle btn2;
    zefir_get_default_button_style(&btn2);
    btn2.text = "Reset";
    btn2.font_size = 14;
    btn2.corner_radius = 10;
    
    // Button 3: blue border
    ZefirButtonStyle btn3;
    zefir_get_default_button_style(&btn3);
    btn3.text = "Blue";
    btn3.font_size = 14;
    btn3.border_color[0] = 0.2f;
    btn3.border_color[1] = 0.4f;
    btn3.border_color[2] = 0.9f;
    
    // Main loop
    while (!zefir_should_close(ctx)) {
        zefir_begin_frame(ctx);
        
        // Click counter +1
        if (zefir_button_ex(ctx, 100, 60, 200, 50, &btn1)) {
            clicks++;
            sprintf(text, "Clicks: %d", clicks);
            printf("Click! Total: %d\n", clicks);
        }
        
        // Reset to 0
        if (zefir_button_ex(ctx, 100, 130, 90, 40, &btn2)) {
            clicks = 0;
            sprintf(text, "Clicks: 0");
        }
        
        // Add 10
        if (zefir_button_ex(ctx, 210, 130, 90, 40, &btn3)) {
            clicks += 10;
            sprintf(text, "Clicks: %d", clicks);
        }
        
        // Display counter and title
        zefir_label(ctx, text, 150, 200);
        zefir_label(ctx, "Try different buttons!", 110, 30);
        
        zefir_end_frame(ctx);
    }
    
    zefir_shutdown(ctx);
    return 0;
}