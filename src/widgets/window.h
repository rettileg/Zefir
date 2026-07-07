#ifndef WIDGET_WINDOW_H
#define WIDGET_WINDOW_H

#include "zefir.h"

typedef struct ZefirGuiWindow ZefirGuiWindow;

ZefirGuiWindow* zefir_window_create(const char* title, float x, float y, float w, float h);
void zefir_window_destroy(ZefirGuiWindow* win);
bool zefir_window_begin(ZefirGuiWindow* win, ZefirContext* ctx);
void zefir_window_end(ZefirGuiWindow* win, ZefirContext* ctx);
void zefir_window_get_pos(ZefirGuiWindow* win, float* x, float* y);
void zefir_window_get_size(ZefirGuiWindow* win, float* w, float* h);

#endif