/**
 * main.cpp — M5Stack PaperS3 Home-Assistant dashboard
 *
 * Boot sequence
 * ─────────────
 *  1. Serial
 *  2. epdiy panel + LVGL display driver
 *  3. GT911 touch (I2C) + LVGL input driver
 *  4. WiFi (blocking with timeout)
 *  5. MQTT (non-blocking; reconnects in loop)
 *  6. Build LVGL UI
 *
 * Loop
 * ────
 *  • lv_task_handler() every ~5 ms  (LVGL rendering + event dispatch)
 *  • mqtt_loop()                     (keepalive + incoming message dispatch)
 *  • LVGL tick advanced via lv_conf.h esp_timer macro (no manual call needed)
 */

#include <Arduino.h>
#include <M5Unified.h>
#include <WiFi.h>
#include <esp_log.h>

#include "config.h"
#include "power/battery.h"
#include "power/sleep.h"
#include "display/epd_driver.h"
#include "display/ui.h"
#include "touch/gt911.h"
#include "mqtt/ha_mqtt.h"
#include "time/rtc_sync.h"
#include "ota/ota.h"

static const char *TAG = "main";

// ─── WiFi ─────────────────────────────────────────────────────────────────────
static void wifi_connect() {
    WiFi.mode(WIFI_STA);
    const uint32_t timeout_ms = 15000;
    Serial.printf("[WiFi] connecting to %s", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    uint32_t start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < timeout_ms) {
        delay(250);
        Serial.print('.');
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("\n[WiFi] connected — IP %s\n",
                      WiFi.localIP().toString().c_str());
    } else {
        Serial.println("\n[WiFi] timeout — continuing without network");
    }
}

// ─── Boot splash (drawn before UI is ready) ───────────────────────────────────
static void draw_boot_message(const char *msg) {
    // Simple raw epdiy text — we can't use LVGL yet at this point.
    // Just print to Serial; the panel stays white until ui_init() completes.
    Serial.printf("[boot] %s\n", msg);
}

// ─── setup() ─────────────────────────────────────────────────────────────────
void setup() {
    // ── 0. Power hold — MUST be first; keeps board on when running on battery ─
    battery_init();

    // ── 1. M5Unified — initialises display, touch, IMU, RTC ─────────────────
    auto cfg = M5.config();
    M5.begin(cfg);

    Serial.begin(115200);
    // Only wait for serial monitor when running on USB power (dev mode).
    // On battery the board boots immediately without stalling.
    if (battery_usb_connected()) {
        uint32_t t = millis();
        while (!Serial && millis() - t < 1500) delay(10);
    }
    Serial.println("\n\n=== PaperS3 Dashboard booting ===");

    // ── 2. LVGL display driver ───────────────────────────────────────────────
    draw_boot_message("init display…");
    epd_driver_init();

    // ── 3. GT911 touch + LVGL input driver ───────────────────────────────────
    draw_boot_message("init touch…");
    gt911_init();

    // ── 3b. Early sleep check ─────────────────────────────────────────────────
    // If this is a post-light-sleep reboot, show the deep-sleep screen and
    // power down immediately — skips WiFi/MQTT/UI entirely.  Never returns.
    sleep_early_check();

    // ── 4. WiFi ───────────────────────────────────────────────────────────────
    draw_boot_message("connecting WiFi…");
    wifi_connect();

    // ── 5. MQTT ───────────────────────────────────────────────────────────────
    draw_boot_message("init MQTT…");
    mqtt_init();

    // ── 6. Load sleep prefs before UI so settings page shows correct values ──
    sleep_module_init();

    // ── 7. Build UI ───────────────────────────────────────────────────────────
    draw_boot_message("building UI…");
    ui_init();

    // ── 8. NTP sync (UI must exist before rtc_update_ui is called) ───────────
    if (WiFi.status() == WL_CONNECTED) {
        rtc_sync_ntp();
        rtc_update_ui();
    }

    lv_refr_now(lv_disp_get_default());
    epd_driver_full_refresh();

    Serial.println("[main] setup complete");
}

// ─── loop() ──────────────────────────────────────────────────────────────────
void loop() {
    // Poll M5Unified — refreshes touch state, button state, etc.
    M5.update();

    // Reset sleep timer on a new touch press (state transition only)
    static bool s_was_touched = false;
    bool touched = M5.Touch.getCount() > 0;
    if (touched && !s_was_touched) {
        sleep_feed();
    }
    s_was_touched = touched;

    // Check inactivity timeout → sleep if needed
    sleep_tick();

    // Commit e-paper panel when dirty and rate-limit allows.
    epd_driver_tick();

    // LVGL task handler — drives rendering and event callbacks.
    // lv_tick is provided by esp_timer via lv_conf.h, so no lv_tick_inc needed.
    lv_task_handler();

    // MQTT keepalive + incoming message dispatch
    mqtt_loop();

    // OTA web server (no-op when inactive)
    ota_loop();

    uint32_t now = millis();

    // Date — update UI every 60 s (catches midnight rollover)
    static uint32_t last_date_ms = 0;
    if (now - last_date_ms >= 60000 || last_date_ms == 0) {
        last_date_ms = now;
        rtc_update_ui();
    }

    // Battery + WiFi status — update UI every 30 s
    static uint32_t last_bat_ms  = 0;
    static bool     last_wifi    = false;
    if (now - last_bat_ms >= 30000 || last_bat_ms == 0) {
        last_bat_ms = now;
        float v   = battery_voltage();
        uint8_t p = battery_percent();
        bool chg  = battery_charging();
        bool usb  = battery_usb_connected();
        ui_update_battery(v, p, chg, usb);
        mqtt_publish_status(p, (int8_t)WiFi.RSSI());
        Serial.printf("[bat] %.2fV  %u%%  %s  rssi=%d\n",
                      v, p, chg ? "CHG" : (usb ? "USB" : "BATT"), WiFi.RSSI());
    }
    bool wifi_now = (WiFi.status() == WL_CONNECTED);
    if (wifi_now != last_wifi) {
        last_wifi = wifi_now;
        ui_update_wifi(wifi_now);
    }

    // Yield to background tasks (WiFi stack, watchdog)
    delay(5);
}
