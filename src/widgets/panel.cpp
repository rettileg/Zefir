#include "panel.h"
#include "../renderer.h"

void panel_get_default_style(ZefirPanelStyle* style) {
    if (!style) return;
    
    style->border_width = 1.5f;
    style->border_color[0] = 0.65f;
    style->border_color[1] = 0.62f;
    style->border_color[2] = 0.58f;
    style->border_color[3] = 1.0f;
    
    style->highlight_color[0] = 1.0f;
    style->highlight_color[1] = 1.0f;
    style->highlight_color[2] = 1.0f;
    style->highlight_color[3] = 0.4f;
    
    style->shadow_color[0] = 0.55f;
    style->shadow_color[1] = 0.52f;
    style->shadow_color[2] = 0.48f;
    style->shadow_color[3] = 0.6f;
    
    style->corner_radius = 3.0f;
}

void panel_draw(ZefirContext* ctx, float x, float y, float w, float h, const ZefirPanelStyle* style) {
    if (!ctx || !style) return;
    
    Renderer* renderer = (Renderer*)zefir_get_renderer(ctx);
    const float* bg_color = zefir_get_bg_color(ctx);
    
    if (!renderer || !bg_color) return;
    
    renderer_draw_rect_ex(renderer, x, y, w, h,
                          bg_color[0], bg_color[1], bg_color[2], 1.0f,
                          style->corner_radius, 2,  // 2 = inset
                          style->border_width,
                          style->border_color,
                          style->highlight_color,
                          style->shadow_color);
}