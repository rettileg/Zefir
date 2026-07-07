/**
 * Zefir GUI - Layout System
 * 
 * Automatic positioning of widgets in rows (HBox) or columns (VBox).
 * 
 * Usage:
 *   ZefirLayout* vbox = zefir_layout_create(x, y, w, h, 1); // 1 = VBox
 *   zefir_layout_begin(vbox);
 *   float x, y, w, h;
 *   while (zefir_layout_next(vbox, &x, &y, &w, &h, 30)) {
 *       zefir_button(ctx, x, y, w, h);
 *   }
 *   zefir_layout_end(vbox);
 *   zefir_layout_destroy(vbox);
 */

#ifndef LAYOUT_H
#define LAYOUT_H

#include <stdbool.h>

typedef struct ZefirLayout ZefirLayout;

/** Create a layout. type: 0 = HBox (horizontal), 1 = VBox (vertical) */
ZefirLayout* zefir_layout_create(float x, float y, float w, float h, int type);

/** Free layout */
void zefir_layout_destroy(ZefirLayout* layout);

/** Start positioning cycle */
void zefir_layout_begin(ZefirLayout* layout);

/** End positioning cycle */
void zefir_layout_end(ZefirLayout* layout);

/** Get next slot position. Returns false when no more space. */
bool zefir_layout_next(ZefirLayout* layout, float* x, float* y, float* w, float* h, float size);

/** Reset cursor to start */
void zefir_layout_reset(ZefirLayout* layout);

/** Set gap between widgets (default 4) */
void zefir_layout_set_spacing(ZefirLayout* layout, float spacing);

/** Set inner padding (default 8, 8) */
void zefir_layout_set_padding(ZefirLayout* layout, float pad_x, float pad_y);

#endif