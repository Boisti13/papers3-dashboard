/**
 * sleep.cpp — Two-stage sleep for PaperS3.
 *
 * Stage 1 (light sleep): triggered after inactivity_ms of no touch.
 *   - No standby screen — e-paper retains the current page image.
 *   - Touch wakeup: GT911 coords injected into LVGL; tapped widget fires.
 *   - Timer wakeup: sets s_pending_deep_sleep flag and returns to main loop.
 *
 * Stage 2 (deep sleep): handled from sleep_tick() in the main loop.
 *   - Called after WiFi reconnects (or 15 s timeout) so NTP date is fresh.
 *   - Deep-sleep screen shown, then M5.Power.powerOff() called from loop().
 *   - Power button cold-boots the device.
 *
 * Timeouts are persisted in NVS via Preferences ("paperdash" namespace).
 */

#include "sleep.h"
#include "../display/epd_driver.h"
#include "../display/ui.h"
#include "../touch/gt911.h"

#include <Arduino.h>
#include <M5Unified.h>
#include <WiFi.h>
#include <lvgl.h>
#include <esp_sleep.h>
#include <driver/gpio.h>
#include <Preferences.h>
#include <time.h>

// ── Config ────────────────────────────────────────────────────────────────────
#define WIFI_WAIT_MS  (15UL * 1000)   // max wait for WiFi before power off

// ── State ─────────────────────────────────────────────────────────────────────
static uint32_t  s_inactivity_ms      = 60000UL;   // 1 min default
static uint32_t  s_lightsleep_ms      = 300000UL;  // 5 min default
static bool      s_inhibit            = false;
static void    (*s_pre_sleep_cb)()    = nullptr;
static uint32_t  s_last_activity      = 0;
static uint32_t  s_light_sleep_entry  = 0;
static bool      s_pending_deep_sleep = false;
static uint32_t  s_deep_pending_since = 0;
static lv_obj_t *s_deep_scr           = nullptr;

// ── NVS helpers ───────────────────────────────────────────────────────────────
static void prefs_load() {
    Preferences p;
    p.begin("paperdash", /*readOnly=*/true);
    s_inactivity_ms = p.getUInt("inact_ms", 60000UL);
    s_lightsleep_ms = p.getUInt("ls_ms",   300000UL);
    p.end();
}

static void prefs_save_inact() {
    Preferences p;
    p.begin("paperdash", false);
    p.putUInt("inact_ms", s_inactivity_ms);
    p.end();
}

static void prefs_save_ls() {
    Preferences p;
    p.begin("paperdash", false);
    p.putUInt("ls_ms", s_lightsleep_ms);
    p.end();
}

// ── Helper: date string ───────────────────────────────────────────────────────
static void get_date_str(char *buf, size_t len) {
    struct tm ti;
    if (getLocalTime(&ti))
        strftime(buf, len, "%A, %d %B", &ti);
    else
        buf[0] = '\0';
}

// ── Deep-sleep standby screen ─────────────────────────────────────────────────
static lv_point_t s_arrow_shaft[2];
static lv_point_t s_arrow_wing1[2];
static lv_point_t s_arrow_wing2[2];

static lv_obj_t *make_arrow_line(lv_obj_t *parent, lv_point_t *pts) {
    lv_obj_t *line = lv_line_create(parent);
    lv_line_set_points(line, pts, 2);
    lv_obj_set_style_line_width(line, 6, 0);
    lv_obj_set_style_line_color(line, lv_color_black(), 0);
    lv_obj_set_style_line_rounded(line, true, 0);
    return line;
}

static void build_deep_standby_screen() {
    s_deep_scr = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(s_deep_scr, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(s_deep_scr, LV_OPA_COVER, 0);
    lv_obj_clear_flag(s_deep_scr, LV_OBJ_FLAG_SCROLLABLE);

    char datebuf[32] = "";
    get_date_str(datebuf, sizeof(datebuf));

    lv_obj_t *lbl_date = lv_label_create(s_deep_scr);
    lv_obj_set_style_text_font(lbl_date, &lv_font_montserrat_36, 0);
    lv_obj_set_style_text_color(lbl_date, lv_color_black(), 0);
    lv_label_set_text(lbl_date, datebuf);
    lv_obj_align(lbl_date, LV_ALIGN_TOP_MID, 0, 100);

    // Horizontal arrow: shaft left→right, tip at (480, 880)
    // Wings at ±30° from tip: upper-left (402,835) and lower-left (402,925)
    s_arrow_shaft[0] = {  60, 880 };
    s_arrow_shaft[1] = { 480, 880 };
    make_arrow_line(s_deep_scr, s_arrow_shaft);

    s_arrow_wing1[0] = { 480, 880 };
    s_arrow_wing1[1] = { 402, 835 };
    make_arrow_line(s_deep_scr, s_arrow_wing1);

    s_arrow_wing2[0] = { 480, 880 };
    s_arrow_wing2[1] = { 402, 925 };
    make_arrow_line(s_deep_scr, s_arrow_wing2);

    lv_obj_t *lbl_btn = lv_label_create(s_deep_scr);
    lv_obj_set_style_text_font(lbl_btn, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(lbl_btn, lv_color_black(), 0);
    lv_label_set_text(lbl_btn, "Press button to wake");
    lv_obj_set_pos(lbl_btn, 60, 820);
}

// ── Stage 1: light sleep ──────────────────────────────────────────────────────
static void do_light_sleep() {
    if (s_light_sleep_entry == 0) {
        s_light_sleep_entry = millis();
        Serial.println("[sleep] entering light sleep");
        Serial.flush();
        if (s_pre_sleep_cb) s_pre_sleep_cb();
        ui_show_sleep_indicator(true);
        lv_refr_now(lv_disp_get_default());
        epd_driver_full_refresh();
    }

    uint32_t elapsed_ms   = millis() - s_light_sleep_entry;
    uint32_t remaining_ms = (elapsed_ms >= s_lightsleep_ms)
                              ? 0
                              : (s_lightsleep_ms - elapsed_ms);

    if (remaining_ms == 0) {
        s_pending_deep_sleep = true;
        s_deep_pending_since = millis();
        s_light_sleep_entry  = 0;
        return;
    }

    uint64_t timer_us = (uint64_t)remaining_ms * 1000ULL;
    esp_sleep_enable_timer_wakeup(timer_us);
    gpio_wakeup_enable((gpio_num_t)48, GPIO_INTR_LOW_LEVEL);
    esp_sleep_enable_gpio_wakeup();

    // WiFi modem sleep must be enabled so the CPU can actually enter light sleep.
    // Without this, esp_light_sleep_start() returns immediately while WiFi is up.
    WiFi.setSleep(true);

    esp_light_sleep_start();

    // ── resumes here after wakeup ─────────────────────────────────────────────
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();

    if (cause == ESP_SLEEP_WAKEUP_TIMER) {
        s_pending_deep_sleep = true;
        s_deep_pending_since = millis();
        s_light_sleep_entry  = 0;
    } else {
        M5.update();
        int16_t tx = 0, ty = 0;
        bool has_touch = gt911_read_raw(&tx, &ty);
        if (has_touch) {
            // Real touch — hide indicator, restore full WiFi power, reset timers
            ui_show_sleep_indicator(false);
            WiFi.setSleep(false);
            gt911_inject_tap(tx, ty);
            s_last_activity     = millis();
            s_light_sleep_entry = 0;
        }
        // Spurious GPIO: don't reset timers; sleep_tick() re-enters with countdown intact.
    }
}

// ── Stage 2: power off — called from sleep_tick() in the main loop ────────────
static void do_deep_sleep() {
    Serial.println("[sleep] powering off");
    Serial.flush();
    build_deep_standby_screen();
    lv_scr_load(s_deep_scr);
    lv_refr_now(lv_disp_get_default());
    epd_driver_full_refresh();
    M5.Power.powerOff();
    // never returns
}

// ── Public API ────────────────────────────────────────────────────────────────
void sleep_early_check() { /* no-op */ }

void sleep_module_init() {
    prefs_load();
    s_last_activity = millis();
    Serial.printf("[sleep] ready — inact=%lums ls=%lums\n",
                  (unsigned long)s_inactivity_ms, (unsigned long)s_lightsleep_ms);
}

void sleep_feed() {
    s_last_activity = millis();
}

void sleep_set_inactivity_ms(uint32_t ms) {
    s_inactivity_ms = ms;
    prefs_save_inact();
}

void sleep_set_lightsleep_ms(uint32_t ms) {
    s_lightsleep_ms = ms;
    prefs_save_ls();
}

uint32_t sleep_get_inactivity_ms() { return s_inactivity_ms; }
uint32_t sleep_get_lightsleep_ms() { return s_lightsleep_ms; }

void sleep_inhibit(bool inhibit) { s_inhibit = inhibit; }

void sleep_set_pre_sleep_cb(void (*cb)(void)) { s_pre_sleep_cb = cb; }

void sleep_tick() {
    // Stage 2: waiting to power off
    if (s_pending_deep_sleep) {
        bool wifi_ok   = (WiFi.status() == WL_CONNECTED);
        bool timed_out = (millis() - s_deep_pending_since >= WIFI_WAIT_MS);
        if (wifi_ok || timed_out) {
            do_deep_sleep();
        }
        return;
    }

    // OTA or other inhibitor active — skip sleep entirely
    if (s_inhibit) return;

    // Stage 1: inactivity → light sleep
    if (millis() - s_last_activity >= s_inactivity_ms) {
        do_light_sleep();
    }
}
