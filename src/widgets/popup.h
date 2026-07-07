#ifndef WIDGET_POPUP_H
#define WIDGET_POPUP_H

#include "zefir.h"

void* popup_create(const char* title, int width, int height);
void popup_destroy(void* popup);
void popup_begin(void* popup);
void popup_end(void* popup);
bool popup_should_close(void* popup);
void* popup_get_native_handle(void* popup);

#endif