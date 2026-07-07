/**
 * Zefir GUI - Main API Header
 * 
 * All public functions, widget styles and types.
 * Include this file to use the framework.
 */

#ifndef ZEFIR_H
#define ZEFIR_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ======== Opaque Types ========
typedef struct ZefirContext ZefirContext;
typedef struct ZefirGuiWindow ZefirGuiWindow;
typedef struct ZefirViewport ZefirViewport;

// ======== Button ========
typedef struct {
    float border_width;
    float border_color[4];
    float highlight_color[4];
    float shadow_color[4];
    float corner_radius;
    float press_offset;
    float hover_brightness;
    float press_darkness;
    float anim_speed_press;
    float anim_speed_release;
    const char* text;
    float font_size;
    float text_color[4];
} ZefirButtonStyle;

// ======== Panel ========
typedef struct {
    float border_width;
    float border_color[4];
    float highlight_color[4];
    float shadow_color[4];
    float corner_radius;
} ZefirPanelStyle;

// ======== Label ========
typedef struct {
    float font_size;
    float color[4];
} ZefirLabelStyle;

// ======== TextBox ========
typedef struct {
    float border_width;
    float border_color[4];
    float highlight_color[4];
    float shadow_color[4];
    float corner_radius;
    float font_size;
    float text_color[4];
    float cursor_color[4];
    float cursor_width;
    int max_length;
} ZefirTextBoxStyle;

// ======== Slider ========
typedef struct {
    float track_height;
    float handle_radius;
    float track_color[4];
    float fill_color[4];
    float handle_color[4];
    float border_width;
    float border_color[4];
    bool show_value;
    float font_size;
} ZefirSliderStyle;

// ======== CheckBox ========
typedef struct {
    float box_size;
    float check_color[4];
    float border_width;
    float border_color[4];
    float hover_color[4];
    float corner_radius;
    const char* text;
    float font_size;
    float text_color[4];
} ZefirCheckBoxStyle;

// ======== RadioButton ========
typedef struct {
    float circle_size;
    float dot_radius;
    float border_width;
    float border_color[4];
    float check_color[4];
    float hover_color[4];
    float corner_radius;
    const char* text;
    float font_size;
    float text_color[4];
} ZefirRadioStyle;

// ======== ComboBox ========
typedef struct {
    float item_height;
    float font_size;
    float text_color[4];
    float bg_color[4];
    float hover_color[4];
    float border_color[4];
    float corner_radius;
    int max_visible_items;
} ZefirComboStyle;

// ======== ProgressBar ========
typedef struct {
    float height;
    float border_width;
    float border_color[4];
    float bg_color[4];
    float fill_color[4];
    float corner_radius;
    bool show_percent;
    float font_size;
    float text_color[4];
} ZefirProgressStyle;

// ======== TabView ========
typedef struct {
    float tab_height;
    float font_size;
    float text_color[4];
    float active_tab_color[4];
    float inactive_tab_color[4];
    float border_color[4];
    float corner_radius;
    float spacing;
} ZefirTabViewStyle;

// ======== ScrollBar ========
typedef struct {
    float track_width;
    float thumb_min_size;
    float track_color[4];
    float thumb_color[4];
    float thumb_hover_color[4];
    float border_color[4];
    float corner_radius;
} ZefirScrollBarStyle;

// ======== Viewport ========
typedef struct {
    float border_width;
    float border_color[4];
    float corner_radius;
    bool own_gl_context;
} ZefirViewportStyle;

// ======== Core Functions ========
ZefirContext*  zefir_init(const char* title, int width, int height);
void           zefir_shutdown(ZefirContext* ctx);
bool           zefir_should_close(ZefirContext* ctx);
void           zefir_begin_frame(ZefirContext* ctx);
void           zefir_end_frame(ZefirContext* ctx);
void           zefir_set_background_color(ZefirContext* ctx, float r, float g, float b, float a);
double         zefir_get_time(ZefirContext* ctx);
void           zefir_get_size(ZefirContext* ctx, int* width, int* height);
void           zefir_get_mouse_pos(ZefirContext* ctx, float* x, float* y);
void*          zefir_get_native_handle(ZefirContext* ctx);

// ======== Button API ========
void zefir_get_default_button_style(ZefirButtonStyle* style);
bool zefir_button(ZefirContext* ctx, float x, float y, float w, float h);
bool zefir_button_ex(ZefirContext* ctx, float x, float y, float w, float h, const ZefirButtonStyle* style);

// ======== Panel API ========
void zefir_get_default_panel_style(ZefirPanelStyle* style);
void zefir_panel(ZefirContext* ctx, float x, float y, float w, float h);
void zefir_panel_ex(ZefirContext* ctx, float x, float y, float w, float h, const ZefirPanelStyle* style);

// ======== Label API ========
void zefir_get_default_label_style(ZefirLabelStyle* style);
void zefir_label(ZefirContext* ctx, const char* text, float x, float y);
void zefir_label_ex(ZefirContext* ctx, const char* text, float x, float y, const ZefirLabelStyle* style);

// ======== TextBox API ========
void zefir_get_default_textbox_style(ZefirTextBoxStyle* style);
bool zefir_textbox(ZefirContext* ctx, float x, float y, float w, float h, char* buffer, int buffer_size);
bool zefir_textbox_ex(ZefirContext* ctx, float x, float y, float w, float h, char* buffer, int buffer_size, const ZefirTextBoxStyle* style);

// ======== Slider API ========
void  zefir_get_default_slider_style(ZefirSliderStyle* style);
float zefir_slider(ZefirContext* ctx, float x, float y, float w, float h, float value, float min, float max);
float zefir_slider_ex(ZefirContext* ctx, float x, float y, float w, float h, float value, float min, float max, const ZefirSliderStyle* style);

// ======== CheckBox API ========
void zefir_get_default_checkbox_style(ZefirCheckBoxStyle* style);
bool zefir_checkbox(ZefirContext* ctx, float x, float y, const char* text, bool* checked);
bool zefir_checkbox_ex(ZefirContext* ctx, float x, float y, float size, const char* text, bool* checked, const ZefirCheckBoxStyle* style);

// ======== RadioButton API ========
void zefir_get_default_radio_style(ZefirRadioStyle* style);
bool zefir_radio(ZefirContext* ctx, float x, float y, const char* text, int* selected, int index);
bool zefir_radio_ex(ZefirContext* ctx, float x, float y, float size, const char* text, int* selected, int index, const ZefirRadioStyle* style);

// ======== ComboBox API ========
void zefir_get_default_combo_style(ZefirComboStyle* style);
int  zefir_combo(ZefirContext* ctx, float x, float y, float w, const char** items, int count, int* selected, const ZefirComboStyle* style);

// ======== ProgressBar API ========
void zefir_get_default_progress_style(ZefirProgressStyle* style);
void zefir_progress_bar(ZefirContext* ctx, float x, float y, float w, float progress, const ZefirProgressStyle* style);

// ======== TabView API ========
void zefir_get_default_tabview_style(ZefirTabViewStyle* style);
int  zefir_tabview_begin(ZefirContext* ctx, float x, float y, float w, float h, const char** tabs, int count, int* active, const ZefirTabViewStyle* style);
void zefir_tabview_end(ZefirContext* ctx);

// ======== ScrollBar API ========
void  zefir_get_default_scrollbar_style(ZefirScrollBarStyle* style);
float zefir_scrollbar_v(ZefirContext* ctx, float x, float y, float w, float h, float* scroll, float content_h, float view_h, const ZefirScrollBarStyle* style);
float zefir_scrollbar_h(ZefirContext* ctx, float x, float y, float w, float h, float* scroll, float content_w, float view_w, const ZefirScrollBarStyle* style);

// ======== Context Menu API ========
bool zefir_context_menu_begin(ZefirContext* ctx);
void zefir_context_menu_end(ZefirContext* ctx);
bool zefir_context_menu_item(ZefirContext* ctx, const char* text);
void zefir_context_menu_separator(ZefirContext* ctx);

// ======== Tooltip API ========
void zefir_tooltip(ZefirContext* ctx, const char* text, float x, float y);

// ======== Popup API ========
void* zefir_popup_create(const char* title, int w, int h);
void  zefir_popup_destroy(void* popup);
void  zefir_popup_begin(void* popup);
void  zefir_popup_end(void* popup);
bool  zefir_popup_should_close(void* popup);
void* zefir_popup_get_native_handle(void* popup);

// ======== Movable Window API ========
ZefirGuiWindow* zefir_window_create(const char* title, float x, float y, float w, float h);
void            zefir_window_destroy(ZefirGuiWindow* win);
bool            zefir_window_begin(ZefirGuiWindow* win, ZefirContext* ctx);
void            zefir_window_end(ZefirGuiWindow* win, ZefirContext* ctx);
void            zefir_window_get_pos(ZefirGuiWindow* win, float* x, float* y);
void            zefir_window_get_size(ZefirGuiWindow* win, float* w, float* h);

// ======== Viewport API ========
ZefirViewport* zefir_viewport_create(ZefirContext* ctx, float x, float y, float w, float h);
void           zefir_viewport_destroy(ZefirViewport* vp);
void*          zefir_viewport_get_native_handle(ZefirViewport* vp);
void           zefir_viewport_begin(ZefirViewport* vp);
void           zefir_viewport_end(ZefirViewport* vp);
void           zefir_viewport_get_size(ZefirViewport* vp, int* w, int* h);
void           zefir_viewport_set_rect(ZefirViewport* vp, float x, float y, float w, float h);
void           zefir_viewport_update(ZefirViewport* vp, float x, float y, float w, float h);

// ======== Texture API ========
unsigned int zefir_load_texture(const char* path);
void         zefir_texture_rect(ZefirContext* ctx, unsigned int tex, float x, float y, float w, float h);
void         zefir_texture_rect_ex(ZefirContext* ctx, unsigned int tex, float x, float y, float w, float h, float scale);

// ======== Drawing API ========
void zefir_draw_rect(ZefirContext* ctx, float x, float y, float w, float h, float r, float g, float b, float a);

// ======== Input Helpers ========
bool zefir_is_hovering(ZefirContext* ctx, float x, float y, float w, float h);
bool zefir_mouse_clicked(ZefirContext* ctx);

// ======== Internal (for widgets) ========
void*         zefir_get_renderer(ZefirContext* ctx);
void*         zefir_get_input(ZefirContext* ctx);
void*         zefir_get_font(ZefirContext* ctx);
const float*  zefir_get_bg_color(ZefirContext* ctx);

#ifdef __cplusplus
}
#endif

#endif // ZEFIR_H