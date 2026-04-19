#include "settings_page.h"
#include "../../styles.h"
#include "../../nav.h"
#include "../../epd_driver.h"
#include "../../../power/sleep.h"
#include "../../../ota/ota.h"

#include <WiFi.h>
#include <stdio.h>

static lv_obj_t *s_page_settings   = nullptr;
static lv_obj_t *s_lbl_inact_val   = nullptr;
static lv_obj_t *s_lbl_ls_val      = nullptr;
static lv_obj_t *s_btn_ota         = nullptr;
static lv_obj_t *s_lbl_ota_status  = nullptr;
static lv_obj_t *s_lbl_wifi_status = nullptr;
static lv_obj_t *s_lbl_wifi_ssid   = nullptr;
static lv_obj_t *s_lbl_wifi_rssi   = nullptr;
static lv_obj_t *s_lbl_wifi_ip     = nullptr;

// ── Label updaters ────────────────────────────────────────────────────────────
static void update_inact_label() {
    if (!s_lbl_inact_val) return;
    char buf[12];
    snprintf(buf, sizeof(buf), "%lu min", (unsigned long)sleep_get_inactivity_ms() / 60000UL);
    lv_label_set_text(s_lbl_inact_val, buf);
}

static void update_ls_label() {
    if (!s_lbl_ls_val) return;
    char buf[12];
    snprintf(buf, sizeof(buf), "%lu min", (unsigned long)sleep_get_lightsleep_ms() / 60000UL);
    lv_label_set_text(s_lbl_ls_val, buf);
}

// ── Callbacks ─────────────────────────────────────────────────────────────────
static void cb_inact_minus(lv_event_t *e) {
    (void)e;
    uint32_t ms = sleep_get_inactivity_ms();
    if (ms > 60000UL) sleep_set_inactivity_ms(ms - 60000UL);
    update_inact_label();
}
static void cb_inact_plus(lv_event_t *e) {
    (void)e;
    uint32_t ms = sleep_get_inactivity_ms();
    if (ms < 30UL * 60000UL) sleep_set_inactivity_ms(ms + 60000UL);
    update_inact_label();
}
static void cb_ls_minus(lv_event_t *e) {
    (void)e;
    uint32_t ms = sleep_get_lightsleep_ms();
    if (ms > 60000UL) sleep_set_lightsleep_ms(ms - 60000UL);
    update_ls_label();
}
static void cb_ls_plus(lv_event_t *e) {
    (void)e;
    uint32_t ms = sleep_get_lightsleep_ms();
    if (ms < 60UL * 60000UL) sleep_set_lightsleep_ms(ms + 60000UL);
    update_ls_label();
}

static void ota_btn_set_active(bool active) {
    if (!s_btn_ota) return;
    lv_obj_t *lbl = lv_obj_get_child(s_btn_ota, 0);

    if (active) {
        lv_obj_set_style_bg_color(s_btn_ota, lv_color_make(0x20, 0x20, 0x20), 0);
        lv_obj_set_style_bg_opa(s_btn_ota, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(s_btn_ota, 4, 0);
        if (lbl) {
            lv_label_set_text(lbl, "Deactivate OTA");
            lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
        }
    } else {
        lv_obj_set_style_bg_color(s_btn_ota, lv_color_white(), 0);
        lv_obj_set_style_bg_opa(s_btn_ota, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(s_btn_ota, 2, 0);
        if (lbl) {
            lv_label_set_text(lbl, "Activate OTA Update");
            lv_obj_set_style_text_color(lbl, lv_color_black(), 0);
        }
        if (s_lbl_ota_status) lv_label_set_text(s_lbl_ota_status, "");
    }
}

static void cb_ota(lv_event_t *e) {
    (void)e;

    if (ota_is_active()) {
        // Deactivate
        ota_deactivate();
        ota_btn_set_active(false);
    } else {
        // Activate
        bool ok = ota_activate();
        if (!ok) {
            if (s_lbl_ota_status)
                lv_label_set_text(s_lbl_ota_status, "WiFi not connected");
        } else {
            ota_btn_set_active(true);
            if (s_lbl_ota_status) {
                char buf[36];
                snprintf(buf, sizeof(buf), "http://%s/", ota_get_ip());
                lv_label_set_text(s_lbl_ota_status, buf);
            }
        }
    }

    // Force immediate e-paper refresh so the button state is visible right away
    lv_refr_now(lv_disp_get_default());
    epd_driver_full_refresh();
}

static void cb_wifi_reconnect(lv_event_t *e) {
    (void)e;
    WiFi.reconnect();
}

// ── Stepper row helper ────────────────────────────────────────────────────────
// Creates: [label_text]              [−] [value] [+]
// Returns the value label so the caller can store and update it.
static lv_obj_t *make_stepper_row(lv_obj_t *parent, int32_t y,
                                   const char *label_text,
                                   lv_event_cb_t cb_minus,
                                   lv_event_cb_t cb_plus,
                                   const char *init_val) {
    // Content width inside card: EPD_WIDTH - page_PAD*2 - card_PAD*2
    const int32_t inner_w = EPD_WIDTH - PAD * 4;   // 476
    const int32_t btn_w   = 60;
    const int32_t btn_h   = 50;
    const int32_t val_w   = 100;
    const int32_t gap     = 8;
    const int32_t plus_x  = inner_w - btn_w;                // 416
    const int32_t val_x   = plus_x - gap - val_w;           // 308
    const int32_t minus_x = val_x - gap - btn_w;            // 240
    const int32_t lbl_vc  = y + (btn_h - 20) / 2;          // vertically center 20pt text

    // Row label
    lv_obj_t *lbl = lv_label_create(parent);
    lv_obj_add_style(lbl, &g_sty_lbl_md, 0);
    lv_label_set_text(lbl, label_text);
    lv_obj_set_pos(lbl, 0, lbl_vc);

    // [−] button
    lv_obj_t *btn_m = lv_btn_create(parent);
    lv_obj_remove_style_all(btn_m);
    lv_obj_add_style(btn_m, &g_sty_btn, 0);
    lv_obj_set_size(btn_m, btn_w, btn_h);
    lv_obj_set_pos(btn_m, minus_x, y);
    lv_obj_add_event_cb(btn_m, cb_minus, LV_EVENT_CLICKED, nullptr);
    {
        lv_obj_t *l = lv_label_create(btn_m);
        lv_obj_add_style(l, &g_sty_lbl_lg, 0);
        lv_label_set_text(l, "-");
        lv_obj_center(l);
    }

    // Value label (centred in its column)
    lv_obj_t *val = lv_label_create(parent);
    lv_obj_add_style(val, &g_sty_lbl_md, 0);
    lv_obj_set_style_text_align(val, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(val, val_w);
    lv_label_set_text(val, init_val);
    lv_obj_set_pos(val, val_x, lbl_vc);

    // [+] button
    lv_obj_t *btn_p = lv_btn_create(parent);
    lv_obj_remove_style_all(btn_p);
    lv_obj_add_style(btn_p, &g_sty_btn, 0);
    lv_obj_set_size(btn_p, btn_w, btn_h);
    lv_obj_set_pos(btn_p, plus_x, y);
    lv_obj_add_event_cb(btn_p, cb_plus, LV_EVENT_CLICKED, nullptr);
    {
        lv_obj_t *l = lv_label_create(btn_p);
        lv_obj_add_style(l, &g_sty_lbl_lg, 0);
        lv_label_set_text(l, "+");
        lv_obj_center(l);
    }

    return val;
}

// ── Page builder ──────────────────────────────────────────────────────────────
void build_page_settings() {
    s_page_settings = make_page();

    const int32_t card_w   = EPD_WIDTH - PAD * 2;   // 508
    const int32_t inner_w  = card_w - PAD * 2;       // 476 (card content)

    // ── Card 1: Sleep Timeouts ────────────────────────────────────────────────
    // heading y=0 (28pt ~34px), row1 y=48, row2 y=108, card outer=200
    const int32_t row1_y        = 48;
    const int32_t row2_y        = 108;
    const int32_t sleep_card_h  = 200;

    lv_obj_t *card1 = lv_obj_create(s_page_settings);
    lv_obj_remove_style_all(card1);
    lv_obj_add_style(card1, &g_sty_card, 0);
    lv_obj_set_size(card1, card_w, sleep_card_h);
    lv_obj_set_pos(card1, 0, 0);
    lv_obj_clear_flag(card1, LV_OBJ_FLAG_SCROLLABLE);

    {
        lv_obj_t *h = lv_label_create(card1);
        lv_obj_add_style(h, &g_sty_lbl_lg, 0);
        lv_label_set_text(h, "Sleep Timeouts");
        lv_obj_set_pos(h, 0, 0);

        char buf[12];

        snprintf(buf, sizeof(buf), "%lu min",
                 (unsigned long)sleep_get_inactivity_ms() / 60000UL);
        s_lbl_inact_val = make_stepper_row(card1, row1_y,
                                            "Inactivity:",
                                            cb_inact_minus, cb_inact_plus, buf);

        snprintf(buf, sizeof(buf), "%lu min",
                 (unsigned long)sleep_get_lightsleep_ms() / 60000UL);
        s_lbl_ls_val = make_stepper_row(card1, row2_y,
                                         "Power Off:",
                                         cb_ls_minus, cb_ls_plus, buf);
    }

    // ── Card 2: OTA Update ────────────────────────────────────────────────────
    // heading y=0, button y=48 h=70, status label y=130, card outer=200
    const int32_t ota_btn_h  = 70;
    const int32_t ota_card_h = 200;
    const int32_t ota_card_y = sleep_card_h + GAP;

    lv_obj_t *card2 = lv_obj_create(s_page_settings);
    lv_obj_remove_style_all(card2);
    lv_obj_add_style(card2, &g_sty_card, 0);
    lv_obj_set_size(card2, card_w, ota_card_h);
    lv_obj_set_pos(card2, 0, ota_card_y);
    lv_obj_clear_flag(card2, LV_OBJ_FLAG_SCROLLABLE);

    {
        lv_obj_t *h = lv_label_create(card2);
        lv_obj_add_style(h, &g_sty_lbl_lg, 0);
        lv_label_set_text(h, "OTA Update");
        lv_obj_set_pos(h, 0, 0);

        s_btn_ota = lv_btn_create(card2);
        lv_obj_remove_style_all(s_btn_ota);
        lv_obj_add_style(s_btn_ota, &g_sty_btn, 0);
        lv_obj_set_size(s_btn_ota, inner_w, ota_btn_h);
        lv_obj_set_pos(s_btn_ota, 0, 48);
        lv_obj_add_event_cb(s_btn_ota, cb_ota, LV_EVENT_CLICKED, nullptr);
        {
            lv_obj_t *l = lv_label_create(s_btn_ota);
            lv_obj_add_style(l, &g_sty_lbl_lg, 0);
            lv_label_set_text(l, "Activate OTA Update");
            lv_obj_center(l);
        }

        s_lbl_ota_status = lv_label_create(card2);
        lv_obj_add_style(s_lbl_ota_status, &g_sty_lbl_md, 0);
        lv_label_set_text(s_lbl_ota_status, "");
        lv_obj_set_pos(s_lbl_ota_status, 0, 48 + ota_btn_h + 12);
    }

    // ── Card 3: WiFi ──────────────────────────────────────────────────────────
    const int32_t wifi_card_y = ota_card_y + ota_card_h + GAP;
    const int32_t wifi_card_h = 280;
    const int32_t row_h       = 36;
    const int32_t val_x       = 160;

    lv_obj_t *card3 = lv_obj_create(s_page_settings);
    lv_obj_remove_style_all(card3);
    lv_obj_add_style(card3, &g_sty_card, 0);
    lv_obj_set_size(card3, card_w, wifi_card_h);
    lv_obj_set_pos(card3, 0, wifi_card_y);
    lv_obj_clear_flag(card3, LV_OBJ_FLAG_SCROLLABLE);

    {
        lv_obj_t *h = lv_label_create(card3);
        lv_obj_add_style(h, &g_sty_lbl_lg, 0);
        lv_label_set_text(h, "WiFi");
        lv_obj_set_pos(h, 0, 0);

        // Row helper — key label left, value label right
        auto make_row = [&](int32_t y, const char *key, const char *val) -> lv_obj_t * {
            lv_obj_t *k = lv_label_create(card3);
            lv_obj_add_style(k, &g_sty_lbl_md, 0);
            lv_label_set_text(k, key);
            lv_obj_set_pos(k, 0, y);

            lv_obj_t *v = lv_label_create(card3);
            lv_obj_add_style(v, &g_sty_lbl_md, 0);
            lv_label_set_text(v, val);
            lv_obj_set_pos(v, val_x, y);
            return v;
        };

        s_lbl_wifi_status = make_row(44,  "Status:", "—");
        s_lbl_wifi_ssid   = make_row(44 + row_h,     "SSID:",   "—");
        s_lbl_wifi_rssi   = make_row(44 + row_h * 2, "Signal:", "—");
        s_lbl_wifi_ip     = make_row(44 + row_h * 3, "IP:",     "—");

        lv_obj_t *btn = lv_btn_create(card3);
        lv_obj_remove_style_all(btn);
        lv_obj_add_style(btn, &g_sty_btn, 0);
        lv_obj_set_size(btn, inner_w, 60);
        lv_obj_set_pos(btn, 0, 44 + row_h * 4 + 8);
        lv_obj_add_event_cb(btn, cb_wifi_reconnect, LV_EVENT_CLICKED, nullptr);
        {
            lv_obj_t *l = lv_label_create(btn);
            lv_obj_add_style(l, &g_sty_lbl_lg, 0);
            lv_label_set_text(l, "Reconnect");
            lv_obj_center(l);
        }
    }
}

lv_obj_t *get_page_settings() { return s_page_settings; }

void settings_page_update_wifi(bool connected, const char *ssid, int8_t rssi, const char *ip) {
    if (s_lbl_wifi_status) lv_label_set_text(s_lbl_wifi_status, connected ? "Connected" : "Not Connected");
    if (s_lbl_wifi_ssid)   lv_label_set_text(s_lbl_wifi_ssid,   ssid && *ssid ? ssid : "—");
    if (s_lbl_wifi_rssi) {
        if (connected) {
            const char *quality = rssi >= -55 ? "Excellent"
                                : rssi >= -65 ? "Good"
                                : rssi >= -75 ? "Weak"
                                :               "Poor";
            char buf[32];
            snprintf(buf, sizeof(buf), "%d dBm (%s)", rssi, quality);
            lv_label_set_text(s_lbl_wifi_rssi, buf);
        } else {
            lv_label_set_text(s_lbl_wifi_rssi, "—");
        }
    }
    if (s_lbl_wifi_ip) lv_label_set_text(s_lbl_wifi_ip, connected && ip && *ip ? ip : "—");
}

bool settings_page_update_sensor(const char *topic, const char *value) {
    (void)topic; (void)value;
    return false;
}

bool settings_page_update_switch(const char *topic, bool on) {
    (void)topic; (void)on;
    return false;
}
