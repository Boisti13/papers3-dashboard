#pragma once
#include <Arduino.h>

/**
 * Initialise PubSubClient with the broker settings from config.h.
 * Call once from setup() after WiFi is connected.
 */
void mqtt_init();

/**
 * Must be called from loop().  Handles reconnection, keepalive, and
 * dispatching incoming messages to LVGL label/button state updates.
 */
void mqtt_loop();

/**
 * Publish a simple ON/OFF command string.
 * topic — full MQTT topic string (use the CMD topics from config.h)
 * value — "ON" or "OFF"
 */
void mqtt_publish(const char *topic, const char *value);

/**
 * Publish a floating-point value (e.g. climate setpoint).
 */
void mqtt_publish_float(const char *topic, float value);

/**
 * Publish device telemetry (battery %, WiFi RSSI) to retained state topics.
 * Call from the periodic battery update in loop().
 */
void mqtt_publish_status(uint8_t battery_pct, int8_t rssi);
