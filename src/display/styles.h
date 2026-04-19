#pragma once
#include <lvgl.h>

// ── Global style objects ──────────────────────────────────────────────────────
// All declared extern here; defined and initialised in styles.cpp via build_styles().

extern lv_style_t g_sty_bar;        // header / footer bar (white, no border)
extern lv_style_t g_sty_page;       // content page background (white, PAD padding)
extern lv_style_t g_sty_lbl_sm;     // 14pt label, black
extern lv_style_t g_sty_lbl_md;     // 20pt label, black
extern lv_style_t g_sty_lbl_lg;     // 28pt label, black
extern lv_style_t g_sty_btn;        // normal button (white bg, black 2px border)
extern lv_style_t g_sty_btn_active; // active/on button (dark bg, white text)
extern lv_style_t g_sty_btn_nav;    // footer nav button (smaller padding)
extern lv_style_t g_sty_btn_grid;   // home-grid button (12px radius)
extern lv_style_t g_sty_card;       // info card (1px border, rounded)

/** Call once during ui_init(), before any page is built. */
void build_styles();
