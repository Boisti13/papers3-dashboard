// Stand-ins for the firmware modules the pages call (MQTT, OTA, sleep).
#include "Arduino.h"
#include "WiFi.h"
#include "mqtt/ha_mqtt.h"
#include "ota/ota.h"
#include "power/sleep.h"
#include "esp_timer.h"
SerialStub Serial;
WiFiStub WiFi;
uint32_t millis() { return (uint32_t)(esp_timer_get_time() / 1000); }
void mqtt_init() {}
void mqtt_loop() {}
void mqtt_publish(const char *, const char *) {}
void mqtt_publish_float(const char *, float) {}
void mqtt_publish_status(uint8_t, int8_t) {}
bool mqtt_connected() { return true; }
bool ota_activate() { return true; }
void ota_deactivate() {}
void ota_loop() {}
bool ota_is_active() { return false; }
const char *ota_get_ip() { return "192.168.1.60"; }
void sleep_early_check() {}
void sleep_module_init() {}
void sleep_feed() {}
void sleep_tick() {}
void sleep_set_inactivity_ms(uint32_t) {}
void sleep_set_lightsleep_ms(uint32_t) {}
uint32_t sleep_get_inactivity_ms() { return 60000; }
uint32_t sleep_get_lightsleep_ms() { return 300000; }
void sleep_inhibit(bool) {}
void sleep_set_pre_sleep_cb(void (*)(void)) {}
void epd_driver_full_refresh() {}
void epd_driver_fast_refresh() {}
