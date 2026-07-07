#include "layout.h"
#include <cstdlib>

struct ZefirLayout {
    float x, y, w, h;
    float spacing;
    float pad_x, pad_y;
    float cursor_x, cursor_y;
    int type; // 0=HBox, 1=VBox
    bool active;
};

ZefirLayout* zefir_layout_create(float x, float y, float w, float h, int type) {
    ZefirLayout* l = new ZefirLayout();
    l->x = x; l->y = y; l->w = w; l->h = h;
    l->spacing = 4.0f;
    l->pad_x = 8.0f; l->pad_y = 8.0f;
    l->type = type;
    l->cursor_x = x + l->pad_x;
    l->cursor_y = y + l->pad_y;
    l->active = false;
    return l;
}

void zefir_layout_destroy(ZefirLayout* layout) {
    delete layout;
}

void zefir_layout_begin(ZefirLayout* layout) {
    if (!layout) return;
    layout->cursor_x = layout->x + layout->pad_x;
    layout->cursor_y = layout->y + layout->pad_y;
    layout->active = true;
}

void zefir_layout_end(ZefirLayout* layout) {
    if (!layout) return;
    layout->active = false;
}

bool zefir_layout_next(ZefirLayout* layout, float* x, float* y, float* w, float* h, float size) {
    if (!layout || !layout->active) return false;
    
    if (layout->type == 0) { // HBox
        if (layout->cursor_x + size > layout->x + layout->w - layout->pad_x) return false;
        *x = layout->cursor_x;
        *y = layout->cursor_y;
        *w = size;
        *h = layout->h - layout->pad_y * 2;
        layout->cursor_x += size + layout->spacing;
    } else { // VBox
        if (layout->cursor_y + size > layout->y + layout->h - layout->pad_y) return false;
        *x = layout->cursor_x;
        *y = layout->cursor_y;
        *w = layout->w - layout->pad_x * 2;
        *h = size;
        layout->cursor_y += size + layout->spacing;
    }
    return true;
}

void zefir_layout_reset(ZefirLayout* layout) {
    if (!layout) return;
    layout->cursor_x = layout->x + layout->pad_x;
    layout->cursor_y = layout->y + layout->pad_y;
}

void zefir_layout_set_spacing(ZefirLayout* layout, float spacing) {
    if (layout) layout->spacing = spacing;
}

void zefir_layout_set_padding(ZefirLayout* layout, float pad_x, float pad_y) {
    if (layout) { layout->pad_x = pad_x; layout->pad_y = pad_y; }
}