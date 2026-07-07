#include "zefir.h"
#include <cstdio>
#include <cstring>
#include <cmath>

int main() {
    // Init window 320x420, dark background
    ZefirContext* ctx = zefir_init("Calculator", 320, 420);
    if (!ctx) return -1;
    zefir_set_background_color(ctx, 0.15f, 0.15f, 0.15f, 1);
    
    char display[256] = "0";
    double num1 = 0, num2 = 0;
    char op = 0;
    bool new_input = true;
    
    // Key press handler
    auto press = [&](const char* key) {
        if (key[0] >= '0' && key[0] <= '9') {
            if (new_input) { display[0] = 0; new_input = false; }
            strcat(display, key);
        } else if (key[0] == '.') {
            if (!strchr(display, '.')) strcat(display, ".");
        } else if (strcmp(key, "C") == 0) {
            strcpy(display, "0"); num1 = num2 = 0; op = 0; new_input = true;
        } else if (strcmp(key, "=") == 0) {
            num2 = atof(display);
            switch (op) {
                case '+': num1 += num2; break;
                case '-': num1 -= num2; break;
                case '*': num1 *= num2; break;
                case '/': num1 = num2 ? num1/num2 : 0; break;
            }
            sprintf(display, "%.6g", num1);
            op = 0; new_input = true;
        } else {
            num1 = atof(display);
            op = key[0];
            new_input = true;
        }
    };
    
    // Keypad layout
    const char* keys[] = {"7","8","9","/", "4","5","6","*", "1","2","3","-", "0",".","=","+", "C"};
    
    // Button style - large text, rounded
    ZefirButtonStyle btn;
    zefir_get_default_button_style(&btn);
    btn.font_size = 20;
    btn.corner_radius = 6;
    
    // Display style - white text, large font
    ZefirLabelStyle disp_ls;
    zefir_get_default_label_style(&disp_ls);
    disp_ls.color[0] = 0.9f; disp_ls.color[1] = 0.9f; disp_ls.color[2] = 0.9f;
    disp_ls.font_size = 24;
    
    // Main loop
    while (!zefir_should_close(ctx)) {
        zefir_begin_frame(ctx);
        
        // Display panel
        zefir_panel(ctx, 20, 20, 280, 60);
        zefir_label_ex(ctx, display, 260 - strlen(display)*14, 34, &disp_ls);
        
        // Draw 17 buttons in 4x4 grid + C
        for (int i = 0; i < 17; i++) {
            int row = i / 4, col = i % 4;
            float bx = 20 + col * 70, by = 100 + row * 70;
            btn.text = keys[i];
            if (zefir_button_ex(ctx, bx, by, 60, 55, &btn)) {
                press(keys[i]);
            }
        }
        
        zefir_end_frame(ctx);
    }
    
    zefir_shutdown(ctx);
    return 0;
}