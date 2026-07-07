#ifndef WIDGET_BUTTON_H
#define WIDGET_BUTTON_H

#include "zefir.h"

bool button_draw(ZefirContext* ctx, float x, float y, float w, float h, const ZefirButtonStyle* style);
void button_get_default_style(ZefirButtonStyle* style);

#endif