/**
 * battery.cpp — PaperS3 power management
 *
 * Hardware (confirmed from M5Stack M5PaperS3-UserDemo / M5Unified source):
 *   GPIO 44 — PWR_HOLD: pull HIGH to latch the boost converter on.
 *             Without this the board powers off when USB is removed.
 *   GPIO  4 — CHG_STATE: LOW = actively charging, HIGH = full / disconnected.
 *   GPIO  5 — USB_DET:   HIGH = USB power present.
 *   GPIO  3 — BAT_ADC:   ADC1 channel 2, sits behind a 100k/100k divider
 *             (measures VBAT/2).  With ADC_11db attenuation the input range
 *             is 0–3.9 V, which covers 0–7.8 V on the raw rail — more than
 *             enough for a 3.0–4.2 V LiPo.
 */

#include "battery.h"
#include "../config.h"

#include <Arduino.h>

void battery_init() {
    // ── PWR_HOLD: latch power on immediately ─────────────────────────────────
    pinMode(PIN_PWR_HOLD, OUTPUT);
    digitalWrite(PIN_PWR_HOLD, HIGH);

    // ── Charge-state inputs ───────────────────────────────────────────────────
    pinMode(PIN_CHG_STATE, INPUT);
    pinMode(PIN_USB_DET,   INPUT);

    // ── ADC: 12 dB attenuation → ~3.9 V full-scale on the pin ───────────────
    analogSetPinAttenuation(PIN_ADC_BATTERY, ADC_11db);
}

float battery_voltage() {
    // Average 8 samples to reduce ADC noise
    uint32_t sum = 0;
    for (int i = 0; i < 8; i++) {
        sum += analogReadMilliVolts(PIN_ADC_BATTERY);
    }
    float pin_mv = sum / 8.0f;
    // Undo the 1:2 voltage divider
    return (pin_mv * 2.0f) / 1000.0f;
}

uint8_t battery_percent() {
    float v = battery_voltage();
    if (v >= 4.2f) return 100;
    if (v <= 3.0f) return 0;
    // Linear approximation over 3.0–4.2 V
    return (uint8_t)((v - 3.0f) / 1.2f * 100.0f + 0.5f);
}

bool battery_charging() {
    return (digitalRead(PIN_USB_DET)   == HIGH) &&
           (digitalRead(PIN_CHG_STATE) == LOW);
}

bool battery_usb_connected() {
    return digitalRead(PIN_USB_DET) == HIGH;
}
