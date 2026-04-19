/**
 * gt911.cpp — LVGL 8.x touch input driver backed by M5Unified.
 *
 * M5Unified initialises the GT911 automatically via M5.begin().
 * M5.update() must be called each loop() iteration to refresh touch state.
 */

#include "gt911.h"
#include "../config.h"

#include <M5Unified.h>
#include <lvgl.h>

// ── Synthetic tap injection ───────────────────────────────────────────────────
// Used after light-sleep wake: read the GT911 coordinates once, inject them
// so LVGL fires the correct callbacks without needing a real live touch.
static int16_t s_tap_x     = 0;
static int16_t s_tap_y     = 0;
static uint8_t s_tap_phase = 0;   // 0=idle  1=send PRESSED  2=send RELEASED

void gt911_inject_tap(int16_t x, int16_t y) {
    s_tap_x     = x;
    s_tap_y     = y;
    s_tap_phase = 1;
}

// ─── LVGL input-device callback ──────────────────────────────────────────────
static void touch_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) {
    if (s_tap_phase == 1) {
        // Synthetic press
        data->point.x = s_tap_x;
        data->point.y = s_tap_y;
        data->state   = LV_INDEV_STATE_PRESSED;
        s_tap_phase   = 2;
        return;
    }
    if (s_tap_phase == 2) {
        // Synthetic release — same coords so LVGL generates LV_EVENT_CLICKED
        data->point.x = s_tap_x;
        data->point.y = s_tap_y;
        data->state   = LV_INDEV_STATE_RELEASED;
        s_tap_phase   = 0;
        return;
    }

    // Normal live-touch path
    if (M5.Touch.getCount() > 0) {
        auto tp = M5.Touch.getDetail(0);
        data->point.x = tp.x;
        data->point.y = tp.y;
        data->state   = tp.isPressed() ? LV_INDEV_STATE_PRESSED
                                       : LV_INDEV_STATE_RELEASED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

// ─── Public API ──────────────────────────────────────────────────────────────
bool gt911_read_raw(int16_t *x, int16_t *y) {
    if (M5.Touch.getCount() > 0) {
        auto tp = M5.Touch.getDetail(0);
        *x = tp.x;
        *y = tp.y;
        return true;   // coords valid regardless of pressed state
    }
    return false;
}

void gt911_init() {
    // GT911 is already initialised by M5.begin(); just register LVGL indev.
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type    = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touch_read_cb;
    lv_indev_drv_register(&indev_drv);

    Serial.println("[Touch] LVGL touch driver registered (M5Unified)");
}
