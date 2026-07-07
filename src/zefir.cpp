#include "zefir.h"
#include "renderer.h"
#include "input.h"
#include "widgets/button.h"
#include "widgets/panel.h"
#include "font.h"
#include "widgets/label.h"
#include "widgets/textbox.h"
#include "texture_loader.h"
#include "widgets/texture_rect.h"
#include "widgets/slider.h"
#include "widgets/checkbox.h"
#include "widgets/tooltip.h"
#include "widgets/tabview.h"
#include "widgets/popup.h"
#include "widgets/popup.h"
#include "widgets/scrollbar.h"
#include "widgets/context_menu.h"
#include "widgets/radio.h"
#include "widgets/combo.h"
#include "widgets/progress.h"
#include "iotaWindow.h"
#include <glad/glad.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <chrono>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <GL/gl.h>
#endif

struct ZefirContext {
    iotaWindow* window;
    HGLRC gl_context;
    HDC device_context;
    
    Renderer* renderer;
    Input* input;
    
    float bg_color[4];
    int window_width;
    int window_height;
    
    std::chrono::steady_clock::time_point start_time;
    Font* font;
};

static unsigned int color_to_frame_color(float r, float g, float b) {
    unsigned char red = (unsigned char)(r * 255.0f);
    unsigned char green = (unsigned char)(g * 255.0f);
    unsigned char blue = (unsigned char)(b * 255.0f);
    return (red << 16) | (green << 8) | blue | 0xFF000000;
}

void* zefir_get_renderer(ZefirContext* ctx) { return ctx ? ctx->renderer : nullptr; }
void* zefir_get_input(ZefirContext* ctx) { return ctx ? ctx->input : nullptr; }
const float* zefir_get_bg_color(ZefirContext* ctx) { return ctx ? ctx->bg_color : nullptr; }

extern "C" {
    
ZefirContext* zefir_init(const char* title, int width, int height) {
    printf("zefir_init start\n");
    ZefirContext* ctx = new ZefirContext();

    ctx->window = nullptr;
    ctx->gl_context = nullptr;
    ctx->device_context = nullptr;
    ctx->renderer = nullptr;
    ctx->input = nullptr;
    ctx->font = nullptr;
    ctx->window_width = 0;
    ctx->window_height = 0;
    ctx->bg_color[0] = 0; ctx->bg_color[1] = 0; ctx->bg_color[2] = 0; ctx->bg_color[3] = 0;
    
    ctx->bg_color[0] = 252.0f / 255.0f;
    ctx->bg_color[1] = 249.0f / 255.0f;
    ctx->bg_color[2] = 238.0f / 255.0f;
    ctx->bg_color[3] = 1.0f;
    
    ctx->window_width = width;
    ctx->window_height = height;
    
    printf("before window\n");
    ctx->window = new iotaWindow();
    if (!ctx->window->init(title, width, height)) {
        printf("window init failed\n");
        delete ctx->window;
        delete ctx;
        return nullptr;
    }
    printf("window ok\n");
    
    HWND hwnd = ctx->window->getHwnd();
    ctx->device_context = GetDC(hwnd);
    printf("dc ok\n");
    
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA, 32,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 24, 8, 0,
        PFD_MAIN_PLANE, 0, 0, 0, 0
    };
    
    int pixel_format = ChoosePixelFormat(ctx->device_context, &pfd);
    SetPixelFormat(ctx->device_context, pixel_format, &pfd);
    
    HGLRC temp_context = wglCreateContext(ctx->device_context);
    wglMakeCurrent(ctx->device_context, temp_context);
    
    typedef HGLRC (WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = 
        (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(temp_context);
    
    if (wglCreateContextAttribsARB) {
        const int context_attribs[] = { 0x2091, 4, 0x2092, 6, 0x2094, 0, 0 };
        ctx->gl_context = wglCreateContextAttribsARB(ctx->device_context, nullptr, context_attribs);
    } else {
        ctx->gl_context = wglCreateContext(ctx->device_context);
    }
    
    wglMakeCurrent(ctx->device_context, ctx->gl_context);
    printf("gl context ok\n");
    
    if (!gladLoadGL()) {
        printf("glad failed\n");
        wglDeleteContext(ctx->gl_context);
        ReleaseDC(hwnd, ctx->device_context);
        ctx->window->shutdown();
        delete ctx->window;
        delete ctx;
        return nullptr;
    }
    printf("glad ok\n");
    
    printf("before renderer\n");
    ctx->renderer = renderer_create();
    printf("renderer ok\n");
    
    printf("before input\n");
    ctx->input = input_create();
    printf("input ok\n");
    
    renderer_set_projection(ctx->renderer, width, height);
    ctx->window->setFrameColor(0xFCF9EEFF);
    ctx->start_time = std::chrono::steady_clock::now();

    printf("before font\n");
    ctx->font = font_create_default(16.0f);
    if (ctx->font) {
        font_set_projection(ctx->font, width, height);
        printf("Font ready\n");
    } else {
        printf("Warning: No font\n");
    }
    printf("font ok\n");
    
    glViewport(0, 0, width, height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    printf("zefir_init done\n");
    return ctx;
}

void zefir_shutdown(ZefirContext* ctx) {
    if (ctx) {
        renderer_destroy(ctx->renderer);
        input_destroy(ctx->input);
        
        if (ctx->gl_context) {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(ctx->gl_context);
        }
        if (ctx->device_context && ctx->window) {
            ReleaseDC(ctx->window->getHwnd(), ctx->device_context);
        }
        if (ctx->window) {
            ctx->window->shutdown();
            delete ctx->window;
        }
        delete ctx;
    }
}

bool zefir_should_close(ZefirContext* ctx) {
    return ctx ? ctx->window->shouldClose() : true;
}

void zefir_begin_frame(ZefirContext* ctx) {
    if (!ctx) return;
    
    auto size = ctx->window->getSize();
    int new_width = size.width;
    int new_height = size.height;
    
    if (new_width != ctx->window_width || new_height != ctx->window_height) {
        ctx->window_width = new_width;
        ctx->window_height = new_height;
        glViewport(0, 0, new_width, new_height);
        renderer_set_projection(ctx->renderer, new_width, new_height);
        if (ctx->font) font_set_projection(ctx->font, new_width, new_height);
    }
    
    input_update(ctx->input, ctx->window->getHwnd());
    renderer_clear(ctx->renderer, ctx->bg_color[0], ctx->bg_color[1], ctx->bg_color[2], ctx->bg_color[3]);
}

void zefir_end_frame(ZefirContext* ctx) {
    if (!ctx) return;
    
    extern void combo_render_all(ZefirContext*);
    combo_render_all(ctx);
    
    SwapBuffers(ctx->device_context);
    
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void zefir_set_background_color(ZefirContext* ctx, float r, float g, float b, float a) {
    if (!ctx) return;
    ctx->bg_color[0] = r;
    ctx->bg_color[1] = g;
    ctx->bg_color[2] = b;
    ctx->bg_color[3] = a;
    ctx->window->setFrameColor(color_to_frame_color(r, g, b));
}

double zefir_get_time(ZefirContext* ctx) {
    if (!ctx) return 0.0;
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(now - ctx->start_time).count();
}

void zefir_get_default_button_style(ZefirButtonStyle* style) {
    button_get_default_style(style);
}

bool zefir_button(ZefirContext* ctx, float x, float y, float w, float h) {
    ZefirButtonStyle style;
    button_get_default_style(&style);
    return zefir_button_ex(ctx, x, y, w, h, &style);
}

bool zefir_button_ex(ZefirContext* ctx, float x, float y, float w, float h, const ZefirButtonStyle* style) {
    return button_draw(ctx, x, y, w, h, style);
}

void zefir_get_mouse_pos(ZefirContext* ctx, float* x, float* y) {
    if (ctx) input_get_mouse(ctx->input, x, y);
}

void zefir_get_default_panel_style(ZefirPanelStyle* style) {
    panel_get_default_style(style);
}

void zefir_panel(ZefirContext* ctx, float x, float y, float w, float h) {
    ZefirPanelStyle style;
    panel_get_default_style(&style);
    panel_draw(ctx, x, y, w, h, &style);
}

void zefir_panel_ex(ZefirContext* ctx, float x, float y, float w, float h, const ZefirPanelStyle* style) {
    panel_draw(ctx, x, y, w, h, style);
}

void* zefir_get_font(ZefirContext* ctx) { return ctx ? ctx->font : nullptr; }

void zefir_get_default_label_style(ZefirLabelStyle* style) {
    label_get_default_style(style);
}

void zefir_label(ZefirContext* ctx, const char* text, float x, float y) {
    ZefirLabelStyle style;
    label_get_default_style(&style);
    label_draw(ctx, text, x, y, &style);
}

void zefir_label_ex(ZefirContext* ctx, const char* text, float x, float y, const ZefirLabelStyle* style) {
    label_draw(ctx, text, x, y, style);
}

void zefir_get_default_textbox_style(ZefirTextBoxStyle* style) {
    textbox_get_default_style(style);
}

bool zefir_textbox(ZefirContext* ctx, float x, float y, float w, float h,
                   char* buffer, int buffer_size) {
    ZefirTextBoxStyle style;
    textbox_get_default_style(&style);
    return textbox_draw(ctx, x, y, w, h, buffer, buffer_size, &style);
}

bool zefir_textbox_ex(ZefirContext* ctx, float x, float y, float w, float h,
                      char* buffer, int buffer_size, const ZefirTextBoxStyle* style) {
    return textbox_draw(ctx, x, y, w, h, buffer, buffer_size, style);
}

void* zefir_get_native_handle(ZefirContext* ctx) {
    return ctx ? ctx->window->getHwnd() : nullptr;
}

void zefir_get_size(ZefirContext* ctx, int* width, int* height) {
    if (ctx) {
        *width = ctx->window_width;
        *height = ctx->window_height;
    }
}

unsigned int zefir_load_texture(const char* path) {
    return load_texture(path);
}

void zefir_get_default_slider_style(ZefirSliderStyle* style) {
    slider_get_default_style(style);
}

float zefir_slider(ZefirContext* ctx, float x, float y, float w, float h, float value, float min_val, float max_val) {
    ZefirSliderStyle style;
    slider_get_default_style(&style);
    return slider_draw(ctx, x, y, w, h, value, min_val, max_val, &style);
}

float zefir_slider_ex(ZefirContext* ctx, float x, float y, float w, float h, float value, float min_val, float max_val, const ZefirSliderStyle* style) {
    return slider_draw(ctx, x, y, w, h, value, min_val, max_val, style);
}

void zefir_get_default_checkbox_style(ZefirCheckBoxStyle* style) {
    checkbox_get_default_style(style);
}

bool zefir_checkbox(ZefirContext* ctx, float x, float y, const char* text, bool* checked) {
    ZefirCheckBoxStyle style;
    checkbox_get_default_style(&style);
    style.text = text;
    return checkbox_draw(ctx, x, y, 0, checked, &style);
}

bool zefir_checkbox_ex(ZefirContext* ctx, float x, float y, float size, const char* text, bool* checked, const ZefirCheckBoxStyle* style) {
    ZefirCheckBoxStyle s = *style;
    s.text = text;
    return checkbox_draw(ctx, x, y, size, checked, &s);
}

void zefir_tooltip(ZefirContext* ctx, const char* text, float x, float y) {
    tooltip_draw(ctx, text, x, y);
}

void zefir_get_default_tabview_style(ZefirTabViewStyle* style) {
    tabview_get_default_style(style);
}

int zefir_tabview_begin(ZefirContext* ctx, float x, float y, float w, float h, const char** tabs, int tab_count, int* active_tab, const ZefirTabViewStyle* style) {
    return tabview_draw(ctx, x, y, w, h, tabs, tab_count, active_tab, style);
}

void* zefir_popup_create(const char* title, int width, int height) {
    return popup_create(title, width, height);
}
void zefir_popup_destroy(void* popup) { popup_destroy(popup); }
void zefir_popup_begin(void* popup) { popup_begin(popup); }
void zefir_popup_end(void* popup) { popup_end(popup); }
bool zefir_popup_should_close(void* popup) { return popup_should_close(popup); }
void* zefir_popup_get_native_handle(void* popup) { return popup_get_native_handle(popup); }

void zefir_get_default_scrollbar_style(ZefirScrollBarStyle* style) { scrollbar_get_default_style(style); }
float zefir_scrollbar_v(ZefirContext* ctx, float x, float y, float w, float h, float* scroll, float content_h, float view_h, const ZefirScrollBarStyle* style) {
    return scrollbar_v_draw(ctx, x, y, w, h, scroll, content_h, view_h, style);
}
float zefir_scrollbar_h(ZefirContext* ctx, float x, float y, float w, float h, float* scroll, float content_w, float view_w, const ZefirScrollBarStyle* style) {
    return scrollbar_h_draw(ctx, x, y, w, h, scroll, content_w, view_w, style);
}

bool zefir_context_menu_begin(ZefirContext* ctx) { return context_menu_begin(ctx); }
void zefir_context_menu_end(ZefirContext* ctx) { context_menu_end(ctx); }
bool zefir_context_menu_item(ZefirContext* ctx, const char* text) { return context_menu_item(ctx, text); }
void zefir_context_menu_separator(ZefirContext* ctx) { context_menu_separator(ctx); }

void zefir_get_default_radio_style(ZefirRadioStyle* style) { radio_get_default_style(style); }

bool zefir_radio(ZefirContext* ctx, float x, float y, const char* text, int* selected, int index) {
    ZefirRadioStyle style;
    radio_get_default_style(&style);
    style.text = text;
    return radio_draw(ctx, x, y, 0, text, selected, index, &style);
}

bool zefir_radio_ex(ZefirContext* ctx, float x, float y, float size, const char* text, int* selected, int index, const ZefirRadioStyle* style) {
    ZefirRadioStyle s = *style;
    s.text = text;
    return radio_draw(ctx, x, y, size, text, selected, index, &s);
}

void zefir_get_default_combo_style(ZefirComboStyle* style) { combo_get_default_style(style); }

int zefir_combo(ZefirContext* ctx, float x, float y, float w, const char** items, int item_count, int* selected, const ZefirComboStyle* style) {
    return combo_draw(ctx, x, y, w, items, item_count, selected, style);
}

void zefir_get_default_progress_style(ZefirProgressStyle* style) { progress_get_default_style(style); }
void zefir_progress_bar(ZefirContext* ctx, float x, float y, float w, float progress, const ZefirProgressStyle* style) {
    progress_draw(ctx, x, y, w, progress, style);
}

void zefir_draw_rect(ZefirContext* ctx, float x, float y, float w, float h, float r, float g, float b, float a) {
    Renderer* renderer = (Renderer*)zefir_get_renderer(ctx);
    if (renderer) {
        float dummy[4] = {0,0,0,0};
        float color[4] = {r, g, b, a};
        renderer_draw_rect_ex(renderer, x, y, w, h, r, g, b, a, 8, 1, 2, color, dummy, dummy);
    }
}

bool zefir_is_hovering(ZefirContext* ctx, float x, float y, float w, float h) {
    Input* input = (Input*)zefir_get_input(ctx);
    return input ? input_is_hovering(input, x, y, w, h) : false;
}

bool zefir_mouse_clicked(ZefirContext* ctx) {
    Input* input = (Input*)zefir_get_input(ctx);
    return input ? input_mouse_released(input) : false;
}

} // extern "C"