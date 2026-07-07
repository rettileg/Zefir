#ifndef WIDGET_LABEL_H
#define WIDGET_LABEL_H

#include "zefir.h"

void label_get_default_style(ZefirLabelStyle* style);
void label_draw(ZefirContext* ctx, const char* text, float x, float y, const ZefirLabelStyle* style);

#endif