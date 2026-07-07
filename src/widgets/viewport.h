#ifndef WIDGET_VIEWPORT_H
#define WIDGET_VIEWPORT_H

#include "zefir.h"

typedef struct ZefirViewport ZefirViewport;

ZefirViewport* zefir_viewport_create(ZefirContext* ctx, float x, float y, float w, float h);
void zefir_viewport_destroy(ZefirViewport* vp);
void* zefir_viewport_get_native_handle(ZefirViewport* vp);
void zefir_viewport_begin(ZefirViewport* vp);
void zefir_viewport_end(ZefirViewport* vp);
void zefir_viewport_get_size(ZefirViewport* vp, int* w, int* h);
void zefir_viewport_set_rect(ZefirViewport* vp, float x, float y, float w, float h);
void zefir_viewport_update(ZefirViewport* vp, float x, float y, float w, float h);

#endif