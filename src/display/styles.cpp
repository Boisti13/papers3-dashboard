#include "styles.h"
#include "nav.h"    // for PAD constant

// ── Style object definitions ──────────────────────────────────────────────────
lv_style_t g_sty_bar;
lv_style_t g_sty_page;
lv_style_t g_sty_lbl_sm;
lv_style_t g_sty_lbl_md;
lv_style_t g_sty_lbl_lg;
lv_style_t g_sty_btn;
lv_style_t g_sty_btn_active;
lv_style_t g_sty_btn_nav;
lv_style_t g_sty_btn_grid;
lv_style_t g_sty_card;

void build_styles() {
    lv_style_init(&g_sty_bar);
    lv_style_set_radius(&g_sty_bar, 0);
    lv_style_set_pad_all(&g_sty_bar, 0);
    lv_style_set_bg_color(&g_sty_bar, lv_color_white());
    lv_style_set_bg_opa(&g_sty_bar, LV_OPA_COVER);
    lv_style_set_border_width(&g_sty_bar, 0);

    lv_style_init(&g_sty_page);
    lv_style_set_radius(&g_sty_page, 0);
    lv_style_set_pad_all(&g_sty_page, PAD);
    lv_style_set_bg_color(&g_sty_page, lv_color_white());
    lv_style_set_bg_opa(&g_sty_page, LV_OPA_COVER);
    lv_style_set_border_width(&g_sty_page, 0);

    lv_style_init(&g_sty_lbl_sm);
    lv_style_set_text_font(&g_sty_lbl_sm, &lv_font_montserrat_14);
    lv_style_set_text_color(&g_sty_lbl_sm, lv_color_black());

    lv_style_init(&g_sty_lbl_md);
    lv_style_set_text_font(&g_sty_lbl_md, &lv_font_montserrat_20);
    lv_style_set_text_color(&g_sty_lbl_md, lv_color_black());

    lv_style_init(&g_sty_lbl_lg);
    lv_style_set_text_font(&g_sty_lbl_lg, &lv_font_montserrat_28);
    lv_style_set_text_color(&g_sty_lbl_lg, lv_color_black());

    lv_style_init(&g_sty_btn);
    lv_style_set_radius(&g_sty_btn, 8);
    lv_style_set_pad_all(&g_sty_btn, 10);
    lv_style_set_bg_color(&g_sty_btn, lv_color_white());
    lv_style_set_bg_opa(&g_sty_btn, LV_OPA_COVER);
    lv_style_set_border_color(&g_sty_btn, lv_color_black());
    lv_style_set_border_width(&g_sty_btn, 2);
    lv_style_set_text_font(&g_sty_btn, &lv_font_montserrat_20);
    lv_style_set_text_color(&g_sty_btn, lv_color_black());
    lv_style_set_shadow_width(&g_sty_btn, 0);
    lv_style_set_outline_width(&g_sty_btn, 0);

    lv_style_init(&g_sty_btn_active);
    lv_style_set_radius(&g_sty_btn_active, 8);
    lv_style_set_pad_all(&g_sty_btn_active, 10);
    lv_style_set_bg_color(&g_sty_btn_active, lv_color_make(0xF4, 0xF4, 0xF4));
    lv_style_set_bg_opa(&g_sty_btn_active, LV_OPA_COVER);
    lv_style_set_border_color(&g_sty_btn_active, lv_color_black());
    lv_style_set_border_width(&g_sty_btn_active, 4);
    lv_style_set_text_font(&g_sty_btn_active, &lv_font_montserrat_28);
    lv_style_set_text_color(&g_sty_btn_active, lv_color_black());
    lv_style_set_shadow_width(&g_sty_btn_active, 0);
    lv_style_set_outline_width(&g_sty_btn_active, 0);

    lv_style_init(&g_sty_btn_nav);
    lv_style_set_radius(&g_sty_btn_nav, 6);
    lv_style_set_pad_hor(&g_sty_btn_nav, 8);
    lv_style_set_pad_ver(&g_sty_btn_nav, 6);
    lv_style_set_bg_color(&g_sty_btn_nav, lv_color_white());
    lv_style_set_bg_opa(&g_sty_btn_nav, LV_OPA_COVER);
    lv_style_set_border_color(&g_sty_btn_nav, lv_color_black());
    lv_style_set_border_width(&g_sty_btn_nav, 2);
    lv_style_set_text_font(&g_sty_btn_nav, &lv_font_montserrat_20);
    lv_style_set_text_color(&g_sty_btn_nav, lv_color_black());
    lv_style_set_shadow_width(&g_sty_btn_nav, 0);
    lv_style_set_outline_width(&g_sty_btn_nav, 0);

    lv_style_init(&g_sty_btn_grid);
    lv_style_set_radius(&g_sty_btn_grid, 12);
    lv_style_set_pad_all(&g_sty_btn_grid, PAD);
    lv_style_set_bg_color(&g_sty_btn_grid, lv_color_white());
    lv_style_set_bg_opa(&g_sty_btn_grid, LV_OPA_COVER);
    lv_style_set_border_color(&g_sty_btn_grid, lv_color_black());
    lv_style_set_border_width(&g_sty_btn_grid, 2);
    lv_style_set_text_font(&g_sty_btn_grid, &lv_font_montserrat_20);
    lv_style_set_text_color(&g_sty_btn_grid, lv_color_black());
    lv_style_set_shadow_width(&g_sty_btn_grid, 0);
    lv_style_set_outline_width(&g_sty_btn_grid, 0);

    lv_style_init(&g_sty_card);
    lv_style_set_radius(&g_sty_card, 8);
    lv_style_set_pad_all(&g_sty_card, PAD);
    lv_style_set_bg_color(&g_sty_card, lv_color_white());
    lv_style_set_bg_opa(&g_sty_card, LV_OPA_COVER);
    lv_style_set_border_color(&g_sty_card, lv_color_black());
    lv_style_set_border_width(&g_sty_card, 1);
    lv_style_set_shadow_width(&g_sty_card, 0);
}
