#include "zf_theme.h"
#include "zefir.h"
#include <cstring>

static ZefirTheme g_current_theme;

void zefir_theme_get_light(ZefirTheme* t) {
    memset(t, 0, sizeof(*t));
    t->bg_color[0]=0.988f; t->bg_color[1]=0.976f; t->bg_color[2]=0.933f; t->bg_color[3]=1;
    t->panel_bg[0]=0.98f; t->panel_bg[1]=0.97f; t->panel_bg[2]=0.92f; t->panel_bg[3]=1;
    t->text_color[0]=0.15f; t->text_color[1]=0.15f; t->text_color[2]=0.15f; t->text_color[3]=1;
    t->button_color[0]=0.95f; t->button_color[1]=0.94f; t->button_color[2]=0.90f; t->button_color[3]=1;
    t->button_hover[0]=1; t->button_hover[1]=1; t->button_hover[2]=0.98f; t->button_hover[3]=1;
    t->button_press[0]=0.90f; t->button_press[1]=0.89f; t->button_press[2]=0.85f; t->button_press[3]=1;
    t->accent_color[0]=0.2f; t->accent_color[1]=0.5f; t->accent_color[2]=0.9f; t->accent_color[3]=1;
    t->border_color[0]=0.6f; t->border_color[1]=0.58f; t->border_color[2]=0.55f; t->border_color[3]=1;
    t->shadow_color[0]=0.5f; t->shadow_color[1]=0.47f; t->shadow_color[2]=0.43f; t->shadow_color[3]=0.8f;
    t->highlight_color[0]=1; t->highlight_color[1]=1; t->highlight_color[2]=1; t->highlight_color[3]=0.9f;
    t->font_size=16; t->corner_radius=3; t->spacing=4; t->padding=8;
    t->font_path="C:/Windows/Fonts/segoeui.ttf";
}

void zefir_theme_get_dark(ZefirTheme* t) {
    memset(t, 0, sizeof(*t));
    t->bg_color[0]=0.15f; t->bg_color[1]=0.15f; t->bg_color[2]=0.17f; t->bg_color[3]=1;
    t->panel_bg[0]=0.18f; t->panel_bg[1]=0.18f; t->panel_bg[2]=0.20f; t->panel_bg[3]=1;
    t->text_color[0]=0.9f; t->text_color[1]=0.9f; t->text_color[2]=0.9f; t->text_color[3]=1;
    t->button_color[0]=0.22f; t->button_color[1]=0.22f; t->button_color[2]=0.25f; t->button_color[3]=1;
    t->button_hover[0]=0.28f; t->button_hover[1]=0.28f; t->button_hover[2]=0.32f; t->button_hover[3]=1;
    t->button_press[0]=0.18f; t->button_press[1]=0.18f; t->button_press[2]=0.20f; t->button_press[3]=1;
    t->accent_color[0]=0.3f; t->accent_color[1]=0.6f; t->accent_color[2]=1.0f; t->accent_color[3]=1;
    t->border_color[0]=0.35f; t->border_color[1]=0.35f; t->border_color[2]=0.38f; t->border_color[3]=1;
    t->shadow_color[0]=0.05f; t->shadow_color[1]=0.05f; t->shadow_color[2]=0.08f; t->shadow_color[3]=0.9f;
    t->highlight_color[0]=0.4f; t->highlight_color[1]=0.4f; t->highlight_color[2]=0.45f; t->highlight_color[3]=0.5f;
    t->font_size=16; t->corner_radius=3; t->spacing=4; t->padding=8;
    t->font_path="C:/Windows/Fonts/segoeui.ttf";
}

void zefir_theme_get_ocean(ZefirTheme* t) {
    zefir_theme_get_light(t);
    t->accent_color[0]=0.0f; t->accent_color[1]=0.5f; t->accent_color[2]=0.7f; t->accent_color[3]=1;
    t->bg_color[0]=0.9f; t->bg_color[1]=0.95f; t->bg_color[2]=0.98f; t->bg_color[3]=1;
    t->button_color[0]=0.85f; t->button_color[1]=0.92f; t->button_color[2]=1.0f; t->button_color[3]=1;
}

void zefir_theme_get_forest(ZefirTheme* t) {
    zefir_theme_get_light(t);
    t->accent_color[0]=0.1f; t->accent_color[1]=0.6f; t->accent_color[2]=0.2f; t->accent_color[3]=1;
    t->bg_color[0]=0.93f; t->bg_color[1]=0.97f; t->bg_color[2]=0.92f; t->bg_color[3]=1;
}

void zefir_theme_get_sunset(ZefirTheme* t) {
    zefir_theme_get_dark(t);
    t->accent_color[0]=1.0f; t->accent_color[1]=0.5f; t->accent_color[2]=0.1f; t->accent_color[3]=1;
    t->bg_color[0]=0.18f; t->bg_color[1]=0.12f; t->bg_color[2]=0.10f; t->bg_color[3]=1;
    t->panel_bg[0]=0.22f; t->panel_bg[1]=0.15f; t->panel_bg[2]=0.12f; t->panel_bg[3]=1;
}

void zefir_theme_apply(const ZefirTheme* theme) {
    if (theme) g_current_theme = *theme;
}

const ZefirTheme* zefir_theme_current() {
    return &g_current_theme;
}

void zefir_theme_get_button_style(const ZefirTheme* t, ZefirButtonStyle* s) {
    if (!t || !s) return;
    zefir_get_default_button_style(s);
    s->corner_radius = t->corner_radius;
    s->border_color[0]=t->border_color[0]; s->border_color[1]=t->border_color[1];
    s->border_color[2]=t->border_color[2]; s->border_color[3]=t->border_color[3];
    s->shadow_color[0]=t->shadow_color[0]; s->shadow_color[1]=t->shadow_color[1];
    s->shadow_color[2]=t->shadow_color[2]; s->shadow_color[3]=t->shadow_color[3];
    s->highlight_color[0]=t->highlight_color[0]; s->highlight_color[1]=t->highlight_color[1];
    s->highlight_color[2]=t->highlight_color[2]; s->highlight_color[3]=t->highlight_color[3];
}

void zefir_theme_get_panel_style(const ZefirTheme* t, ZefirPanelStyle* s) {
    if (!t || !s) return;
    zefir_get_default_panel_style(s);
    s->corner_radius = t->corner_radius;
    s->border_color[0]=t->border_color[0]; s->border_color[1]=t->border_color[1];
    s->border_color[2]=t->border_color[2]; s->border_color[3]=t->border_color[3];
    s->shadow_color[0]=t->shadow_color[0]; s->shadow_color[1]=t->shadow_color[1];
    s->shadow_color[2]=t->shadow_color[2]; s->shadow_color[3]=t->shadow_color[3];
    s->highlight_color[0]=t->highlight_color[0]; s->highlight_color[1]=t->highlight_color[1];
    s->highlight_color[2]=t->highlight_color[2]; s->highlight_color[3]=t->highlight_color[3];
}

void zefir_theme_get_textbox_style(const ZefirTheme* t, ZefirTextBoxStyle* s) {
    if (!t || !s) return;
    zefir_get_default_textbox_style(s);
    s->corner_radius = t->corner_radius;
    s->text_color[0]=t->text_color[0]; s->text_color[1]=t->text_color[1];
    s->text_color[2]=t->text_color[2]; s->text_color[3]=t->text_color[3];
    s->border_color[0]=t->border_color[0]; s->border_color[1]=t->border_color[1];
    s->border_color[2]=t->border_color[2]; s->border_color[3]=t->border_color[3];
}

void zefir_theme_get_slider_style(const ZefirTheme* t, ZefirSliderStyle* s) {
    if (!t || !s) return;
    zefir_get_default_slider_style(s);
    s->fill_color[0]=t->accent_color[0]; s->fill_color[1]=t->accent_color[1];
    s->fill_color[2]=t->accent_color[2]; s->fill_color[3]=t->accent_color[3];
    s->handle_color[0]=t->button_color[0]; s->handle_color[1]=t->button_color[1];
    s->handle_color[2]=t->button_color[2]; s->handle_color[3]=t->button_color[3];
}

void zefir_theme_get_checkbox_style(const ZefirTheme* t, ZefirCheckBoxStyle* s) {
    if (!t || !s) return;
    zefir_get_default_checkbox_style(s);
    s->check_color[0]=t->accent_color[0]; s->check_color[1]=t->accent_color[1];
    s->check_color[2]=t->accent_color[2]; s->check_color[3]=t->accent_color[3];
    s->border_color[0]=t->border_color[0]; s->border_color[1]=t->border_color[1];
    s->border_color[2]=t->border_color[2]; s->border_color[3]=t->border_color[3];
    s->text_color[0]=t->text_color[0]; s->text_color[1]=t->text_color[1];
    s->text_color[2]=t->text_color[2]; s->text_color[3]=t->text_color[3];
}

void zefir_theme_get_label_style(const ZefirTheme* t, ZefirLabelStyle* s) {
    if (!t || !s) return;
    zefir_get_default_label_style(s);
    s->color[0]=t->text_color[0]; s->color[1]=t->text_color[1];
    s->color[2]=t->text_color[2]; s->color[3]=t->text_color[3];
    s->font_size = t->font_size;
}