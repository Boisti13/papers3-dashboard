/**
 * ui.cpp — thin orchestrator for the PaperS3 dashboard UI.
 *
 * Builds header/footer, initialises navigation, then delegates page
 * construction to per-page modules under pages/.
 * All MQTT routing is dispatched through the page-module update functions.
 */

#include "ui.h"
#include "styles.h"
#include "nav.h"
#include "../config.h"
#include "../fonts/icons.h"

#include "pages/home/home_page.h"
#include "pages/heating/heating_page.h"
#include "pages/switches/switches_page.h"
#include "pages/cleaning/cleaning_page.h"
#include "pages/climate/climate_page.h"
#include "pages/thinklab/thinklab_page.h"
#include "pages/arthur/arthur_page.h"
#include "pages/bedroom/bedroom_page.h"
#include "pages/settings/settings_page.h"

#include <lvgl.h>
#include <stdio.h>
#include <string.h>

// ── Header / footer widget handles ───────────────────────────────────────────
static lv_obj_t *s_lbl_date;
static lv_obj_t *s_lbl_page_name;
static lv_obj_t *s_lbl_wifi;
static lv_obj_t *s_lbl_battery;
static lv_obj_t *s_lbl_sleep;
static lv_obj_t *s_btn_back;

// ── Footer callbacks ──────────────────────────────────────────────────────────
static void cb_nav_back(lv_event_t *e) { navigate_back(); }
static void cb_nav_home(lv_event_t *e) { navigate_home(); }
static void cb_nav_menu(lv_event_t *e) { navigate_to(get_page_settings(), "Settings"); }

// ── Header ────────────────────────────────────────────────────────────────────
static void build_header(lv_obj_t *scr) {
    lv_obj_t *hdr = lv_obj_create(scr);
    lv_obj_remove_style_all(hdr);
    lv_obj_add_style(hdr, &g_sty_bar, 0);
    lv_obj_set_size(hdr, EPD_WIDTH, HDR_H);
    lv_obj_set_pos(hdr, 0, 0);
    lv_obj_set_style_border_side(hdr, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_width(hdr, 2, 0);
    lv_obj_set_style_border_color(hdr, lv_color_black(), 0);
    lv_obj_clear_flag(hdr, LV_OBJ_FLAG_SCROLLABLE);

    s_lbl_date = lv_label_create(hdr);
    lv_obj_add_style(s_lbl_date, &g_sty_lbl_md, 0);
    lv_label_set_text(s_lbl_date, "--. ----");
    lv_obj_align(s_lbl_date, LV_ALIGN_LEFT_MID, PAD, 0);

    s_lbl_page_name = lv_label_create(hdr);
    lv_obj_add_style(s_lbl_page_name, &g_sty_lbl_lg, 0);
    lv_obj_set_style_text_font(s_lbl_page_name, &lv_font_montserrat_36, 0);
    lv_label_set_text(s_lbl_page_name, "Home");
    lv_obj_set_width(s_lbl_page_name, EPD_WIDTH - 240);
    lv_obj_set_style_text_align(s_lbl_page_name, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(s_lbl_page_name, LV_ALIGN_CENTER, 0, 0);

    s_lbl_wifi = lv_label_create(hdr);
    lv_obj_set_style_text_font(s_lbl_wifi, &lv_font_mdi_20, 0);
    lv_obj_set_style_text_color(s_lbl_wifi, lv_color_black(), 0);
    lv_label_set_text(s_lbl_wifi, MDI_WIFI_OFF);
    lv_obj_align(s_lbl_wifi, LV_ALIGN_RIGHT_MID, -PAD, -12);

    s_lbl_battery = lv_label_create(hdr);
    lv_obj_set_style_text_font(s_lbl_battery, &lv_font_mdi_battery_20, 0);
    lv_obj_set_style_text_color(s_lbl_battery, lv_color_black(), 0);
    lv_label_set_text(s_lbl_battery, MDI_BAT_0);
    lv_obj_align(s_lbl_battery, LV_ALIGN_RIGHT_MID, -PAD, 12);

    s_lbl_sleep = lv_label_create(hdr);
    lv_obj_add_style(s_lbl_sleep, &g_sty_lbl_md, 0);
    lv_label_set_text(s_lbl_sleep, "zzz");
    lv_obj_align(s_lbl_sleep, LV_ALIGN_RIGHT_MID, -PAD - 110, 12);
    lv_obj_add_flag(s_lbl_sleep, LV_OBJ_FLAG_HIDDEN);
}

// ── Footer ────────────────────────────────────────────────────────────────────
static void build_footer(lv_obj_t *scr) {
    lv_obj_t *ftr = lv_obj_create(scr);
    lv_obj_remove_style_all(ftr);
    lv_obj_add_style(ftr, &g_sty_bar, 0);
    lv_obj_set_size(ftr, EPD_WIDTH, FTR_H);
    lv_obj_set_pos(ftr, 0, EPD_HEIGHT - FTR_H);
    lv_obj_set_style_border_side(ftr, LV_BORDER_SIDE_TOP, 0);
    lv_obj_set_style_border_width(ftr, 2, 0);
    lv_obj_set_style_border_color(ftr, lv_color_black(), 0);
    lv_obj_clear_flag(ftr, LV_OBJ_FLAG_SCROLLABLE);

    const int32_t btn_w = 110;
    const int32_t btn_h = FTR_H - 16;
    const int32_t btn_y = 8;

    s_btn_back = lv_btn_create(ftr);
    lv_obj_remove_style_all(s_btn_back);
    lv_obj_add_style(s_btn_back, &g_sty_btn_nav, 0);
    lv_obj_set_size(s_btn_back, btn_w, btn_h);
    lv_obj_set_pos(s_btn_back, PAD, btn_y);
    lv_obj_add_event_cb(s_btn_back, cb_nav_back, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *lbl_back = lv_label_create(s_btn_back);
    lv_label_set_text(lbl_back, LV_SYMBOL_LEFT " Back");
    lv_obj_center(lbl_back);
    lv_obj_add_flag(s_btn_back, LV_OBJ_FLAG_HIDDEN);  // hidden until depth > 0

    lv_obj_t *btn_home = lv_btn_create(ftr);
    lv_obj_remove_style_all(btn_home);
    lv_obj_add_style(btn_home, &g_sty_btn_nav, 0);
    lv_obj_set_size(btn_home, btn_w, btn_h);
    lv_obj_set_pos(btn_home, (EPD_WIDTH - btn_w) / 2, btn_y);
    lv_obj_add_event_cb(btn_home, cb_nav_home, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *lbl_home = lv_label_create(btn_home);
    lv_label_set_text(lbl_home, LV_SYMBOL_HOME);
    lv_obj_center(lbl_home);

    lv_obj_t *btn_menu = lv_btn_create(ftr);
    lv_obj_remove_style_all(btn_menu);
    lv_obj_add_style(btn_menu, &g_sty_btn_nav, 0);
    lv_obj_set_size(btn_menu, btn_w, btn_h);
    lv_obj_set_pos(btn_menu, EPD_WIDTH - btn_w - PAD, btn_y);
    lv_obj_add_event_cb(btn_menu, cb_nav_menu, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *lbl_menu = lv_label_create(btn_menu);
    lv_label_set_text(lbl_menu, "\xe2\x80\xa2\xe2\x80\xa2\xe2\x80\xa2");  // •••
    lv_obj_center(lbl_menu);
}

// ── ui_init ───────────────────────────────────────────────────────────────────
void ui_init() {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    build_styles();
    build_header(scr);

    // Content container between header and footer
    lv_obj_t *content = lv_obj_create(scr);
    lv_obj_remove_style_all(content);
    lv_obj_set_size(content, EPD_WIDTH, CONTENT_H);
    lv_obj_set_pos(content, 0, HDR_H);
    lv_obj_set_style_bg_color(content, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(content, LV_OPA_COVER, 0);
    lv_obj_clear_flag(content, LV_OBJ_FLAG_SCROLLABLE);

    build_footer(scr);

    // Register content + navigation widgets
    nav_init(content, s_lbl_page_name, s_btn_back);

    // Build all pages (home must be last — it references the others via getters)
    build_page_heating();
    build_page_switches();
    build_page_cleaning();
    build_page_climate();
    build_page_thinklab();
    build_page_arthur();
    build_page_bedroom();
    build_page_settings();
    build_page_home();

    // Register home page and show it
    nav_set_home(get_page_home());
    navigate_to(get_page_home(), "Home");
}

// ── Public update functions ───────────────────────────────────────────────────

void ui_update_date(const char *date_str) {
    if (s_lbl_date) lv_label_set_text(s_lbl_date, date_str);
}

void ui_update_battery(float volts, uint8_t pct, bool charging, bool usb) {
    if (!s_lbl_battery) return;
    if ((charging || usb) && pct >= 100) {
        lv_label_set_text(s_lbl_battery, MDI_BAT_CHG_FULL);
        return;
    }
    if ((charging || usb) && pct < 100) {
        const char *icon;
        if      (pct < 10)  icon = MDI_BAT_CHG_0;
        else if (pct < 20)  icon = MDI_BAT_CHG_10;
        else if (pct < 30)  icon = MDI_BAT_CHG_20;
        else if (pct < 40)  icon = MDI_BAT_CHG_30;
        else if (pct < 60)  icon = MDI_BAT_CHG_50;
        else if (pct < 70)  icon = MDI_BAT_CHG_60;
        else if (pct < 80)  icon = MDI_BAT_CHG_70;
        else if (pct < 90)  icon = MDI_BAT_CHG_80;
        else                icon = MDI_BAT_CHG_90;
        lv_label_set_text(s_lbl_battery, icon);
    } else {
        static const char *bat[11] = {
            MDI_BAT_EMPTY, MDI_BAT_10, MDI_BAT_20, MDI_BAT_30,
            MDI_BAT_40,    MDI_BAT_50, MDI_BAT_60, MDI_BAT_70,
            MDI_BAT_80,    MDI_BAT_90, MDI_BAT_0   // 100% → battery-outline (F0079)
        };
        uint8_t idx = (pct >= 100) ? 10 : pct / 10;
        lv_label_set_text(s_lbl_battery, bat[idx]);
    }
}

void ui_update_wifi(bool connected, int8_t rssi) {
    if (!s_lbl_wifi) return;
    const char *icon;
    if (!connected) {
        icon = MDI_WIFI_OFF;
    } else if (rssi >= -55) {
        icon = MDI_WIFI_4;
    } else if (rssi >= -65) {
        icon = MDI_WIFI_3;
    } else if (rssi >= -75) {
        icon = MDI_WIFI_2;
    } else {
        icon = MDI_WIFI_1;
    }
    lv_label_set_text(s_lbl_wifi, icon);
}

void ui_update_switch(const char *topic, bool on) {
    if (switches_page_update_switch(topic, on))  return;
    if (cleaning_page_update_switch(topic, on))  return;
    if (bedroom_page_update_switch(topic, on))   return;
    thinklab_page_update_switch(topic, on);
}

void ui_update_sensor(const char *topic, const char *value) {
    if (heating_page_update_sensor(topic, value))  return;
    if (climate_page_update_sensor(topic, value))  return;
    if (thinklab_page_update_sensor(topic, value)) return;
    cleaning_page_update_sensor(topic, value);
}

void ui_update_light(const char *topic, bool on) { ui_update_switch(topic, on); }
void ui_update_climate(const char *topic, float v) { (void)topic; (void)v; }

void ui_show_sleep_indicator(bool show) {
    if (!s_lbl_sleep) return;
    if (show) lv_obj_clear_flag(s_lbl_sleep, LV_OBJ_FLAG_HIDDEN);
    else      lv_obj_add_flag(s_lbl_sleep, LV_OBJ_FLAG_HIDDEN);
}
