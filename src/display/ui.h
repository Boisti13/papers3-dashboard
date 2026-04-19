#pragma once
#include <stdint.h>

/** Initialise LVGL screen, build all pages, show Home. Call once from setup(). */
void ui_init();

/** Update header date string (dd.mm.yyyy). */
void ui_update_date(const char *date_str);

/** Update battery info in header. */
void ui_update_battery(float volts, uint8_t pct, bool charging, bool usb);

/** Update WiFi icon in header — shows signal strength (1–4 bars) or off icon. */
void ui_update_wifi(bool connected, int8_t rssi = 0);

/**
 * Route an MQTT binary (ON/OFF) state update to the correct widget.
 * Handles: switch tiles, vacuum room toggles, ThinkLab service states.
 */
void ui_update_switch(const char *topic, bool on);

/** Alias — light state is handled identically to a switch. */
void ui_update_light(const char *topic, bool on);

/**
 * Route an MQTT string/numeric value to the correct widget.
 * Handles: heating room temps/humidity/setpoint/action/preset/window,
 *          outdoor climate (temp, humidity, CO2, PM2.5),
 *          ThinkLab CPU + disk usage,
 *          vacuum repeat selection.
 */
void ui_update_sensor(const char *topic, const char *value);

/** No-op stub kept for backward compatibility. */
void ui_update_climate(const char *topic, float value);

/** Show or hide the "zzz" sleep indicator in the header. */
void ui_show_sleep_indicator(bool show);
