#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>

typedef struct Renderer Renderer;

Renderer* renderer_create(void);
void renderer_destroy(Renderer* renderer);
void renderer_set_projection(Renderer* renderer, int width, int height);
void renderer_clear(Renderer* renderer, float red, float green, float blue, float alpha);
void renderer_draw_rect(Renderer* renderer, float x, float y, float w, float h, 
                        float red, float green, float blue, float alpha, 
                        float radius, int style);

void renderer_draw_rect_ex(Renderer* renderer, float x, float y, float w, float h,
                           float red, float green, float blue, float alpha,
                           float radius, int style,
                           float border_width,
                           const float border_color[4],
                           const float highlight_color[4],
                           const float shadow_color[4]);

#endif