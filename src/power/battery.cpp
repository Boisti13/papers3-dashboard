/**
 * battery.cpp — PaperS3 power management
 *
 * Uses M5Unified Power API (M5.Power) for all battery/charging reads.
 * GPIO 44 (PWR_HOLD) is still asserted manually — M5Unified does not
 * manage this pin on PaperS3.
 */

#include "battery.h"
#include "../config.h"

#include <Arduino.h>
#include <M5Unified.h>

void battery_init() {
    pinMode(PIN_PWR_HOLD, OUTPUT);
    digitalWrite(PIN_PWR_HOLD, HIGH);
}

float battery_voltage() {
    // getBatteryVoltage() returns millivolts; 0 means not available
    int32_t mv = M5.Power.getBatteryVoltage();
    if (mv <= 0) return 0.0f;
    return mv / 1000.0f;
}

uint8_t battery_percent() {
    int32_t pct = M5.Power.getBatteryLevel();
    if (pct < 0)   return 0;
    if (pct > 100) return 100;
    return (uint8_t)pct;
}

bool battery_charging() {
    return M5.Power.isCharging();
}

bool battery_usb_connected() {
    return M5.Power.isCharging();
}
