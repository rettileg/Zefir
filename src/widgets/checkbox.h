#ifndef WIDGET_CHECKBOX_H
#define WIDGET_CHECKBOX_H

#include "zefir.h"

void checkbox_get_default_style(ZefirCheckBoxStyle* style);
bool checkbox_draw(ZefirContext* ctx, float x, float y, float size, bool* checked, const ZefirCheckBoxStyle* style);

#endif