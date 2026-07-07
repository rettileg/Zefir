#ifndef WIDGET_PROGRESS_H
#define WIDGET_PROGRESS_H

#include "zefir.h"

void progress_get_default_style(ZefirProgressStyle* style);
void progress_draw(ZefirContext* ctx, float x, float y, float w, float progress, const ZefirProgressStyle* style);

#endif