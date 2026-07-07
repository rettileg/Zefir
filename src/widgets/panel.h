#ifndef WIDGET_PANEL_H
#define WIDGET_PANEL_H

#include "zefir.h"

void panel_get_default_style(ZefirPanelStyle* style);
void panel_draw(ZefirContext* ctx, float x, float y, float w, float h, const ZefirPanelStyle* style);

#endif