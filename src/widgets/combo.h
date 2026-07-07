#ifndef WIDGET_COMBO_H
#define WIDGET_COMBO_H

#include "zefir.h"

void combo_get_default_style(ZefirComboStyle* style);
int combo_draw(ZefirContext* ctx, float x, float y, float w, const char** items, int item_count, int* selected, const ZefirComboStyle* style);
void combo_render_all(ZefirContext* ctx);

#endif