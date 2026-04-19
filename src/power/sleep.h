#pragma once
#include <stdint.h>

/**
 * sleep.h — Two-stage sleep for PaperS3.
 *
 * Stage 1 (light sleep): after inactivity timeout (default 1 min).
 *   - E-paper retains current image.
 *   - Touch wakes and fires the tapped widget's action.
 *   - Timer wakes after lightsleep timeout (default 5 min) → Stage 2.
 *
 * Stage 2 (power off): M5.Power.powerOff() after WiFi reconnects (or 15 s).
 *   - Deep-sleep screen shown before poweroff.
 *
 * Timeouts are runtime-configurable and persisted in NVS.
 */

void sleep_early_check();   // no-op, kept for build compatibility
void sleep_module_init();   // call before ui_init() so settings page reads correct values
void sleep_feed();          // call whenever touch activity is detected
void sleep_tick();          // call every loop iteration

// ── Timeout configuration (persisted in NVS) ──────────────────────────────────
void     sleep_set_inactivity_ms(uint32_t ms);   // default 60 000
void     sleep_set_lightsleep_ms(uint32_t ms);   // default 300 000
uint32_t sleep_get_inactivity_ms();
uint32_t sleep_get_lightsleep_ms();

// ── Sleep inhibit (e.g. while OTA is active) ──────────────────────────────────
void sleep_inhibit(bool inhibit);

// ── Pre-sleep callback ────────────────────────────────────────────────────────
// Called once just before the device enters light sleep. Use it to navigate
// away from pages that shouldn't be visible on wake (e.g. Settings, ThinkLab).
void sleep_set_pre_sleep_cb(void (*cb)(void));
