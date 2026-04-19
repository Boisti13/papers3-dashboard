#pragma once
#include <lvgl.h>

/**
 * Initialise the GT911 touch controller over I2C and register it as an
 * LVGL input device.
 *
 * Wiring (M5Stack PaperS3):
 *   SDA : G41    SCL : G42    INT : G48
 *   I2C address: 0x5D (ADDR pin pulled low) or 0x14 (ADDR high)
 *   — M5Stack uses 0x5D; change GT911_ADDR if yours differs.
 */
void gt911_init();

/**
 * Raw read — fills *x, *y, *pressed from the GT911 register bank.
 * Useful for diagnostics; normally called indirectly via the LVGL indev cb.
 */
bool gt911_read_raw(int16_t *x, int16_t *y);

/**
 * Inject a synthetic tap at (x, y) into the LVGL indev pipeline.
 * The touch_read_cb will report PRESSED on the next indev poll, then
 * RELEASED on the one after — exactly like a real finger tap.
 * Call this after waking from light sleep with the coordinates read
 * from the GT911 before M5.update() clears the buffer.
 */
void gt911_inject_tap(int16_t x, int16_t y);
