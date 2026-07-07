/**
 * Zefir GUI - Theme System
 * 
 * Provides 5 built-in themes and functions to apply them to all widgets.
 * 
 * Usage:
 *   ZefirTheme theme;
 *   zefir_theme_get_dark(&theme);
 *   zefir_theme_apply(&theme);
 *   zefir_set_background_color(ctx, theme.bg_color[0], ...);
 * 
 * Available themes:
 *   - zefir_theme_get_light()   - Light beige (252, 249, 238)
 *   - zefir_theme_get_dark()    - Dark gray (38, 38, 43)
 *   - zefir_theme_get_ocean()   - Light blue tint
 *   - zefir_theme_get_forest()  - Light green tint
 *   - zefir_theme_get_sunset()  - Dark warm tones
 */

#ifndef ZF_THEME_H
#define ZF_THEME_H

#include "zefir.h"

/** Theme structure containing all color/style settings */
typedef struct {
    float bg_color[4];          // Window background
    float panel_bg[4];          // Panel background
    float text_color[4];        // Default text color
    float button_color[4];      // Button base color
    float button_hover[4];      // Button hover highlight
    float button_press[4];      // Button press darken
    float accent_color[4];      // Slider fill, checkbox check
    float border_color[4];      // Widget borders
    float shadow_color[4];      // Drop shadows
    float highlight_color[4];   // Top highlights
    float font_size;            // Default font size
    float corner_radius;        // Default corner rounding
    float spacing;              // Default spacing
    float padding;              // Default padding
    const char* font_path;      // Font file path
} ZefirTheme;

// ======== Built-in Themes ========

/** Light beige theme (default) */
void zefir_theme_get_light(ZefirTheme* theme);

/** Dark gray theme */
void zefir_theme_get_dark(ZefirTheme* theme);

/** Ocean blue theme */
void zefir_theme_get_ocean(ZefirTheme* theme);

/** Forest green theme */
void zefir_theme_get_forest(ZefirTheme* theme);

/** Sunset warm dark theme */
void zefir_theme_get_sunset(ZefirTheme* theme);

// ======== Theme Application ========

/** Set as current global theme */
void zefir_theme_apply(const ZefirTheme* theme);

/** Get current theme */
const ZefirTheme* zefir_theme_current();

// ======== Style Getters ========
// Apply theme colors to specific widget styles

void zefir_theme_get_button_style(const ZefirTheme* theme, ZefirButtonStyle* style);
void zefir_theme_get_panel_style(const ZefirTheme* theme, ZefirPanelStyle* style);
void zefir_theme_get_textbox_style(const ZefirTheme* theme, ZefirTextBoxStyle* style);
void zefir_theme_get_slider_style(const ZefirTheme* theme, ZefirSliderStyle* style);
void zefir_theme_get_checkbox_style(const ZefirTheme* theme, ZefirCheckBoxStyle* style);
void zefir_theme_get_label_style(const ZefirTheme* theme, ZefirLabelStyle* style);

#endif