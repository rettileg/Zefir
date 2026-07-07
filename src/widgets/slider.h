#ifndef WIDGET_SLIDER_H
#define WIDGET_SLIDER_H

#include "zefir.h"

void slider_get_default_style(ZefirSliderStyle* style);
float slider_draw(ZefirContext* ctx, float x, float y, float w, float h, float value, float min_val, float max_val, const ZefirSliderStyle* style);

#endif