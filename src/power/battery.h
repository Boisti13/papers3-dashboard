#pragma once
#include <stdint.h>

/**
 * battery_init() — must be called FIRST in setup() before any other init.
 *   • Asserts GPIO 44 (PWR_HOLD) HIGH so the board stays on under battery power.
 *   • Configures ADC + charge-state GPIO inputs.
 */
void battery_init();

/** Raw battery voltage in volts (e.g. 3.95).  Averages 8 ADC samples. */
float battery_voltage();

/** 0–100 % estimate based on 3.0 V (0 %) … 4.2 V (100 %) curve. */
uint8_t battery_percent();

/** True while USB is connected AND CHG_STATE pin reports active charging. */
bool battery_charging();

/** True whenever USB power is present (charging or trickle). */
bool battery_usb_connected();
