#include "input.h"
#include <windows.h>
#include <cstring>

#define MAX_CHAR_QUEUE 64

struct Input {
    float mouse_x, mouse_y;
    bool mouse_curr;
    bool mouse_prev;
    
    bool keys_curr[256];
    bool keys_prev[256];
    
    unsigned int char_queue[MAX_CHAR_QUEUE];
    int char_queue_head;
    int char_queue_tail;
};

Input* input_create() {
    Input* i = new Input();
    memset(i, 0, sizeof(Input));
    return i;
}

void input_destroy(Input* i) { delete i; }

void input_update(Input* i, HWND hwnd) {
    POINT p;
    GetCursorPos(&p);
    ScreenToClient(hwnd, &p);
    
    i->mouse_x = (float)p.x;
    i->mouse_y = (float)p.y;
    
    i->mouse_prev = i->mouse_curr;
    i->mouse_curr = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    
    memcpy(i->keys_prev, i->keys_curr, sizeof(i->keys_curr));
    
    int important_keys[] = {
        VK_BACK, VK_DELETE, VK_LEFT, VK_RIGHT, VK_RETURN, VK_TAB,
        VK_HOME, VK_END, VK_ESCAPE,
        '0','1','2','3','4','5','6','7','8','9',
        'A','B','C','D','E','F','G','H','I','J','K','L','M',
        'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
        VK_OEM_1, VK_OEM_2, VK_OEM_3, VK_OEM_4, VK_OEM_5,
        VK_OEM_6, VK_OEM_7, VK_OEM_8,
        VK_OEM_PERIOD, VK_OEM_COMMA, VK_OEM_MINUS, VK_OEM_PLUS,
        VK_OEM_102,
        VK_SPACE,
        0
    };
    
    for (int j = 0; important_keys[j]; j++) {
        int vk = important_keys[j];
        i->keys_curr[vk] = (GetAsyncKeyState(vk) & 0x8000) != 0;
        
        if (i->keys_curr[vk] && !i->keys_prev[vk]) {
            BYTE key_state[256];
            GetKeyboardState(key_state);
            
            WCHAR unicode[4] = {0};
            HKL layout = GetKeyboardLayout(0);
            
            int result = ToUnicodeEx(vk, MapVirtualKey(vk, MAPVK_VK_TO_VSC),
                                     key_state, unicode, 4, 0, layout);
            
            if (result > 0) {
                for (int u = 0; u < result; u++) {
                    input_push_char(i, unicode[u]);
                }
            }
        }
    }
}

void input_get_mouse(Input* i, float* x, float* y) {
    *x = i->mouse_x;
    *y = i->mouse_y;
}

bool input_mouse_down(Input* i) { return i->mouse_curr; }
bool input_mouse_pressed(Input* i) { return i->mouse_curr && !i->mouse_prev; }
bool input_mouse_released(Input* i) { return !i->mouse_curr && i->mouse_prev; }

bool input_is_hovering(Input* i, float x, float y, float w, float h) {
    return i->mouse_x >= x && i->mouse_x <= x + w &&
           i->mouse_y >= y && i->mouse_y <= y + h;
}

bool input_key_pressed(Input* i, int key) {
    if (key < 0 || key >= 256) return false;
    return i->keys_curr[key] && !i->keys_prev[key];
}

bool input_key_down(Input* i, int key) {
    if (key < 0 || key >= 256) return false;
    return i->keys_curr[key];
}

int input_get_pressed_key(Input* i) {
    int keys[] = {VK_BACK, VK_DELETE, VK_LEFT, VK_RIGHT, VK_RETURN, VK_TAB, VK_HOME, VK_END, 0};
    for (int j = 0; keys[j]; j++) {
        if (i->keys_curr[keys[j]]) return keys[j];
    }
    return 0;
}

void input_push_char(Input* i, unsigned int c) {
    if (i->char_queue_head != i->char_queue_tail) {
        int idx = i->char_queue_tail - 1;
        if (idx < 0) idx = MAX_CHAR_QUEUE - 1;
        if (i->char_queue[idx] == c) return;
    }
    
    int next = (i->char_queue_tail + 1) % MAX_CHAR_QUEUE;
    if (next != i->char_queue_head) {
        i->char_queue[i->char_queue_tail] = c;
        i->char_queue_tail = next;
    }
}

unsigned int input_pop_char(Input* i) {
    if (i->char_queue_head == i->char_queue_tail) return 0;
    unsigned int c = i->char_queue[i->char_queue_head];
    i->char_queue_head = (i->char_queue_head + 1) % MAX_CHAR_QUEUE;
    return c;
}

bool input_has_chars(Input* i) {
    return i->char_queue_head != i->char_queue_tail;
}