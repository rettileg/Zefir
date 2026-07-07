/**
 * Zefir GUI - Menu Bar
 * 
 * Horizontal menu bar at the top of the window.
 * 
 * Usage:
 *   zefir_menu_bar_begin(ctx);
 *   if (zefir_menu_item(ctx, "File")) { ... }
 *   if (zefir_menu_item(ctx, "Edit")) { ... }
 *   zefir_menu_bar_end(ctx);
 */

#ifndef ZF_MENU_H
#define ZF_MENU_H

#include "zefir.h"
#include <stdbool.h>

/** Begin menu bar. Must be called first. */
void zefir_menu_bar_begin(ZefirContext* ctx);

/** Add a menu item. Returns true when clicked. */
bool zefir_menu_item(ZefirContext* ctx, const char* text);

/** End menu bar. */
void zefir_menu_bar_end(ZefirContext* ctx);

#endif