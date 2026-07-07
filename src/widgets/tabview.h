#ifndef WIDGET_TABVIEW_H
#define WIDGET_TABVIEW_H

#include "zefir.h"

void tabview_get_default_style(ZefirTabViewStyle* style);
int tabview_draw(ZefirContext* ctx, float x, float y, float w, float h, const char** tabs, int tab_count, int* active_tab, const ZefirTabViewStyle* style);

#endif