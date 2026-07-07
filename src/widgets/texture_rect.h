#ifndef WIDGET_TEXTURE_RECT_H
#define WIDGET_TEXTURE_RECT_H

#include "zefir.h"

void zefir_texture_rect(ZefirContext* ctx, unsigned int texture_id, float x, float y, float w, float h);
void zefir_texture_rect_ex(ZefirContext* ctx, unsigned int texture_id, float x, float y, float w, float h, float scale);

#endif