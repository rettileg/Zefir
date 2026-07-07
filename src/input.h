#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
#include <windows.h>

typedef struct Input Input;

Input* input_create(void);
void input_destroy(Input* i);
void input_update(Input* i, HWND hwnd);
void input_get_mouse(Input* i, float* x, float* y);
bool input_mouse_down(Input* i);
bool input_mouse_pressed(Input* i);
bool input_mouse_released(Input* i);
bool input_is_hovering(Input* i, float x, float y, float w, float h);

bool input_key_pressed(Input* i, int key);
bool input_key_down(Input* i, int key);
int input_get_pressed_key(Input* i);

void input_push_char(Input* i, unsigned int c);
unsigned int input_pop_char(Input* i);
bool input_has_chars(Input* i);

#endif