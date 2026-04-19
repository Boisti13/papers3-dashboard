#pragma once
#include <lvgl.h>

/** Initialise M5GFX and register the LVGL display + flush callback. */
void epd_driver_init();

/**
 * Call from loop() every iteration.
 * Commits the M5GFX framebuffer to the e-paper panel when the dirty flag is
 * set AND the minimum refresh interval has elapsed.  This decouples the
 * physical panel update from LVGL's flush callback so continuous LVGL renders
 * don't cause continuous panel refreshes.
 */
void epd_driver_tick();

/** Force an immediate full-quality panel refresh (ignores rate limiter). */
void epd_driver_full_refresh();

/** Force an immediate fast panel refresh (ignores rate limiter). */
void epd_driver_fast_refresh();
