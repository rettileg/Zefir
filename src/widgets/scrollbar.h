#ifndef WIDGET_SCROLLBAR_H
#define WIDGET_SCROLLBAR_H

#include "zefir.h"

void scrollbar_get_default_style(ZefirScrollBarStyle* style);
float scrollbar_v_draw(ZefirContext* ctx, float x, float y, float w, float h, float* scroll, float content_h, float view_h, const ZefirScrollBarStyle* style);
float scrollbar_h_draw(ZefirContext* ctx, float x, float y, float w, float h, float* scroll, float content_w, float view_w, const ZefirScrollBarStyle* style);

#endif