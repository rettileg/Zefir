#ifndef WIDGET_TEXTBOX_H
#define WIDGET_TEXTBOX_H

#include "zefir.h"

void textbox_get_default_style(ZefirTextBoxStyle* style);
bool textbox_draw(ZefirContext* ctx, float x, float y, float w, float h, 
                  char* buffer, int buffer_size, const ZefirTextBoxStyle* style);

#endif