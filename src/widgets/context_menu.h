#ifndef WIDGET_CONTEXT_MENU_H
#define WIDGET_CONTEXT_MENU_H

#include "zefir.h"

bool context_menu_begin(ZefirContext* ctx);
void context_menu_end(ZefirContext* ctx);
bool context_menu_item(ZefirContext* ctx, const char* text);
void context_menu_separator(ZefirContext* ctx);

#endif