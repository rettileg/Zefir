#include "zefir.h"
#include <cstdio>
#include <cstring>

int main() {
    // Init window 400x300, dark background
    ZefirContext* ctx = zefir_init("Login", 400, 300);
    if (!ctx) return -1;
    zefir_set_background_color(ctx, 0.2f, 0.2f, 0.2f, 1);
    
    // Input buffers
    char username[256] = "";
    char password[256] = "";
    char message[128] = "";
    
    // White label style (for dark background)
    ZefirLabelStyle ls;
    zefir_get_default_label_style(&ls);
    ls.color[0] = 0.9f; ls.color[1] = 0.9f; ls.color[2] = 0.9f;
    
    // Login button
    ZefirButtonStyle btn;
    zefir_get_default_button_style(&btn);
    btn.text = "Login";
    btn.font_size = 16;
    
    // Main loop
    while (!zefir_should_close(ctx)) {
        zefir_begin_frame(ctx);
        
        // Title
        zefir_label_ex(ctx, "LOGIN", 160, 30, &ls);
        
        // Username field
        zefir_label_ex(ctx, "Username:", 80, 70, &ls);
        zefir_textbox(ctx, 80, 90, 240, 35, username, 256);
        
        // Password field
        zefir_label_ex(ctx, "Password:", 80, 140, &ls);
        zefir_textbox(ctx, 80, 160, 240, 35, password, 256);
        
        // Submit
        if (zefir_button_ex(ctx, 150, 220, 100, 40, &btn)) {
            if (username[0] && password[0]) {
                sprintf(message, "Welcome, %s!", username);
                printf("Login: %s\n", username);
            } else {
                strcpy(message, "Fill all fields!");
            }
        }
        
        // Feedback message
        if (message[0]) zefir_label_ex(ctx, message, 100, 275, &ls);
        
        zefir_end_frame(ctx);
    }
    
    zefir_shutdown(ctx);
    return 0;
}