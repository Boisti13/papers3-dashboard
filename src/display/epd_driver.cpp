/**
 * epd_driver.cpp — M5GFX → LVGL 8.x bridge for M5Stack PaperS3 e-paper.
 *
 * Refresh model
 * ─────────────
 * LVGL calls epd_flush_cb() whenever it has rendered pixels.  The callback
 * writes them into M5GFX's internal framebuffer (fast, no panel I/O) and sets
 * g_dirty.  The actual panel commit (display()) only happens inside
 * epd_driver_tick(), which is called from loop() and enforces a minimum
 * interval between physical refreshes.  This prevents LVGL's continuous
 * internal renders from causing continuous e-paper refreshes.
 */

#include "epd_driver.h"
#include "../config.h"

#include <M5Unified.h>
#include <Arduino.h>
#include <lvgl.h>

// ─── Draw buffer ──────────────────────────────────────────────────────────────
static const size_t DRAW_BUF_LINES  = EPD_HEIGHT / 10;
static const size_t DRAW_BUF_PIXELS = EPD_WIDTH * DRAW_BUF_LINES;

static lv_color_t *draw_buf_1 = nullptr;
static lv_color_t *draw_buf_2 = nullptr;

// ─── Refresh state ────────────────────────────────────────────────────────────
// Minimum ms between physical panel commits.
static const uint32_t EPD_MIN_REFRESH_MS = 2000;

// After this many fast refreshes, do one quality pass to clear ghosting.
static const uint32_t FULL_REFRESH_EVERY = 20;

static bool     g_dirty          = false;
static uint32_t g_last_refresh   = 0;
static uint32_t g_partial_count  = 0;

// ─── LVGL flush callback ──────────────────────────────────────────────────────
// Only writes pixels into M5GFX's framebuffer — never touches the panel.
static void epd_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area,
                          lv_color_t *color_p) {
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;
    M5.Display.pushImage(area->x1, area->y1, w, h, (lgfx::rgb565_t *)color_p);
    if (lv_disp_flush_is_last(drv))
        g_dirty = true;
    lv_disp_flush_ready(drv);
}

// ─── LVGL log forwarding ──────────────────────────────────────────────────────
static void lv_log_cb(const char *buf) { Serial.print(buf); }

// ─── Public API ───────────────────────────────────────────────────────────────
void epd_driver_init() {
    Serial.printf("[EPD] M5GFX display: %dx%d\n",
                  M5.Display.width(), M5.Display.height());

    draw_buf_1 = (lv_color_t *)ps_malloc(DRAW_BUF_PIXELS * sizeof(lv_color_t));
    draw_buf_2 = (lv_color_t *)ps_malloc(DRAW_BUF_PIXELS * sizeof(lv_color_t));
    if (!draw_buf_1 || !draw_buf_2) {
        Serial.println("[EPD] FATAL: PSRAM alloc failed");
        return;
    }

    lv_init();
    lv_log_register_print_cb(lv_log_cb);

    static lv_disp_draw_buf_t draw_buf_dsc;
    lv_disp_draw_buf_init(&draw_buf_dsc, draw_buf_1, draw_buf_2, DRAW_BUF_PIXELS);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res  = (lv_coord_t)M5.Display.width();
    disp_drv.ver_res  = (lv_coord_t)M5.Display.height();
    disp_drv.flush_cb = epd_flush_cb;
    disp_drv.draw_buf = &draw_buf_dsc;
    lv_disp_drv_register(&disp_drv);

    Serial.println("[EPD] LVGL display driver registered");
}

void epd_driver_tick() {
    if (!g_dirty) return;
    if (millis() - g_last_refresh < EPD_MIN_REFRESH_MS) return;

    g_dirty = false;

    if (++g_partial_count >= FULL_REFRESH_EVERY) {
        M5.Display.setEpdMode(lgfx::epd_mode_t::epd_quality);
        M5.Display.display();
        g_partial_count = 0;
    } else {
        M5.Display.setEpdMode(lgfx::epd_mode_t::epd_fastest);
        M5.Display.display();
    }
    M5.Display.waitDisplay();
    g_last_refresh = millis();
}

void epd_driver_full_refresh() {
    M5.Display.setEpdMode(lgfx::epd_mode_t::epd_quality);
    M5.Display.display();
    M5.Display.waitDisplay();
    g_dirty         = false;
    g_last_refresh  = millis();
    g_partial_count = 0;
}

void epd_driver_fast_refresh() {
    M5.Display.setEpdMode(lgfx::epd_mode_t::epd_fastest);
    M5.Display.display();
    M5.Display.waitDisplay();
    g_dirty        = false;
    g_last_refresh = millis();
    ++g_partial_count;
}
