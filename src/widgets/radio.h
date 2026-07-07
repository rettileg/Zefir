#ifndef WIDGET_RADIO_H
#define WIDGET_RADIO_H

#include "zefir.h"

void radio_get_default_style(ZefirRadioStyle* style);
bool radio_draw(ZefirContext* ctx, float x, float y, float size, const char* text, int* selected, int index, const ZefirRadioStyle* style);

#endif