/**
 * ha_mqtt.cpp — PubSubClient wrapper for Home Assistant MQTT integration.
 *
 * On connect:
 *   • LWT set to "offline" on MQTT_AVAIL_TOPIC (retained)
 *   • Publishes "online" to MQTT_AVAIL_TOPIC (retained)
 *   • Publishes MQTT Discovery config for battery + WiFi RSSI sensors
 *   • Subscribes to all HA state topics
 *
 * Incoming messages update LVGL widget state via lv_async_call() so all
 * UI mutations happen on the main task, never inside the MQTT callback.
 */

#include "ha_mqtt.h"
#include "../config.h"
#include "../display/ui.h"

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <lvgl.h>
#include <string.h>

// ─── Client ───────────────────────────────────────────────────────────────────
static WiFiClient   wifi_client;
static PubSubClient mqtt_client(wifi_client);


// ─── Topic classification helpers ────────────────────────────────────────────

// Returns true if this topic carries a binary ON/OFF state
static bool is_binary_topic(const char *topic) {
    // Switch / light states
    if (strcmp(topic, TOPIC_SW_TV_STATE)       == 0) return true;
    if (strcmp(topic, TOPIC_SW_TVSOCK_STATE)   == 0) return true;
    if (strcmp(topic, TOPIC_SW_SHIELD_STATE)   == 0) return true;
    if (strcmp(topic, TOPIC_SW_LIVING_STATE)   == 0) return true;
    if (strcmp(topic, TOPIC_SW_KITCHEN_STATE)  == 0) return true;
    if (strcmp(topic, TOPIC_SW_HEATLAMP_STATE)    == 0) return true;
    // Bedroom lights
    if (strcmp(topic, TOPIC_BED_NIGHTSTAND_STATE) == 0) return true;
    if (strcmp(topic, TOPIC_BED_LIGHT_STATE)      == 0) return true;
    // Vacuum room input_booleans
    if (strcmp(topic, TOPIC_VAC_ROOM_BATH) == 0) return true;
    if (strcmp(topic, TOPIC_VAC_ROOM_HALL) == 0) return true;
    if (strcmp(topic, TOPIC_VAC_ROOM_BED)  == 0) return true;
    if (strcmp(topic, TOPIC_VAC_ROOM_LIVE) == 0) return true;
    if (strcmp(topic, TOPIC_VAC_ROOM_KIDS) == 0) return true;
    if (strcmp(topic, TOPIC_VAC_ROOM_KTCH) == 0) return true;
    // ThinkLab LXC / VM switches
    if (strcmp(topic, TOPIC_TL_LXC_ABS)  == 0) return true;
    if (strcmp(topic, TOPIC_TL_LXC_JEL)  == 0) return true;
    if (strcmp(topic, TOPIC_TL_LXC_PIH)  == 0) return true;
    if (strcmp(topic, TOPIC_TL_LXC_SMB)  == 0) return true;
    if (strcmp(topic, TOPIC_TL_LXC_ZT)   == 0) return true;
    if (strcmp(topic, TOPIC_TL_LXC_SPR)  == 0) return true;
    if (strcmp(topic, TOPIC_TL_VM_HAOS)  == 0) return true;
    return false;
}

// Returns true if this is a window binary_sensor (needs "Open"/"Closed" text mapping)
static bool is_window_topic(const char *topic) {
    if (strcmp(topic, TOPIC_HEAT_BATH_WINDOW) == 0) return true;
    if (strcmp(topic, TOPIC_HEAT_KTCH_WINDOW) == 0) return true;
    if (strcmp(topic, TOPIC_HEAT_BED_WINDOW)  == 0) return true;
    if (strcmp(topic, TOPIC_HEAT_LIVE_WINDOW) == 0) return true;
    if (strcmp(topic, TOPIC_HEAT_ART_WINDOW)  == 0) return true;
    return false;
}

static bool value_is_on(const char *v) {
    return strcasecmp(v, "on") == 0 || strcmp(v, "1") == 0 || strcasecmp(v, "true") == 0;
}

// ─── MQTT message callback ────────────────────────────────────────────────────
static void on_message(char *topic, uint8_t *payload, unsigned int length) {
    char msg[64];
    size_t n = min((size_t)length, sizeof(msg) - 1);
    memcpy(msg, payload, n);
    msg[n] = '\0';
    Serial.printf("[MQTT] %s → %s\n", topic, msg);

    // Call UI updates directly — safe in single-threaded Arduino context.
    // lv_refr_now() forces LVGL to render dirty objects immediately so that
    // g_dirty gets set even when the affected page is currently hidden.
    if (is_binary_topic(topic)) {
        ui_update_switch(topic, value_is_on(msg));
    } else {
        ui_update_sensor(topic, msg);
    }
    lv_refr_now(lv_disp_get_default());
}

// ─── MQTT Discovery ───────────────────────────────────────────────────────────
static void publish_discovery() {
    StaticJsonDocument<768> doc;
    char buf[768];

    // Battery sensor
    doc.clear();
    JsonObject dev = doc.createNestedObject("device");
    dev["identifiers"][0] = MQTT_DEVICE_ID;
    dev["name"]           = "PaperS3 Dashboard";
    dev["manufacturer"]   = "M5Stack";
    dev["model"]          = "PaperS3 (C139)";
    doc["name"]                = "Battery";
    doc["unique_id"]           = MQTT_DEVICE_ID "_battery";
    doc["state_topic"]         = MQTT_BAT_STATE;
    doc["availability_topic"]  = MQTT_AVAIL_TOPIC;
    doc["unit_of_measurement"] = "%";
    doc["device_class"]        = "battery";
    doc["state_class"]         = "measurement";
    serializeJson(doc, buf, sizeof(buf));
    mqtt_client.publish("homeassistant/sensor/" MQTT_DEVICE_ID "_battery/config",
                        buf, /*retain=*/true);

    // WiFi RSSI sensor
    doc.clear();
    dev = doc.createNestedObject("device");
    dev["identifiers"][0] = MQTT_DEVICE_ID;
    dev["name"]           = "PaperS3 Dashboard";
    dev["manufacturer"]   = "M5Stack";
    dev["model"]          = "PaperS3 (C139)";
    doc["name"]                = "WiFi RSSI";
    doc["unique_id"]           = MQTT_DEVICE_ID "_wifi_rssi";
    doc["state_topic"]         = MQTT_WIFI_STATE;
    doc["availability_topic"]  = MQTT_AVAIL_TOPIC;
    doc["unit_of_measurement"] = "dBm";
    doc["device_class"]        = "signal_strength";
    doc["state_class"]         = "measurement";
    doc["entity_category"]     = "diagnostic";
    serializeJson(doc, buf, sizeof(buf));
    mqtt_client.publish("homeassistant/sensor/" MQTT_DEVICE_ID "_wifi_rssi/config",
                        buf, /*retain=*/true);

    Serial.println("[MQTT] discovery published");
}

// ─── Subscriptions ────────────────────────────────────────────────────────────
static void subscribe_all() {
    // ── Heating rooms (5 rooms × 6 topics each) ───────────────────────────────
    mqtt_client.subscribe(TOPIC_HEAT_BATH_TEMP);
    mqtt_client.subscribe(TOPIC_HEAT_BATH_HUM);
    mqtt_client.subscribe(TOPIC_HEAT_BATH_TARGET);
    mqtt_client.subscribe(TOPIC_HEAT_BATH_ACTION);
    mqtt_client.subscribe(TOPIC_HEAT_BATH_PRESET);
    mqtt_client.subscribe(TOPIC_HEAT_BATH_WINDOW);
    mqtt_client.subscribe(TOPIC_HEAT_BATH_MODE);

    mqtt_client.subscribe(TOPIC_HEAT_KTCH_TEMP);
    mqtt_client.subscribe(TOPIC_HEAT_KTCH_HUM);
    mqtt_client.subscribe(TOPIC_HEAT_KTCH_TARGET);
    mqtt_client.subscribe(TOPIC_HEAT_KTCH_ACTION);
    mqtt_client.subscribe(TOPIC_HEAT_KTCH_PRESET);
    mqtt_client.subscribe(TOPIC_HEAT_KTCH_WINDOW);
    mqtt_client.subscribe(TOPIC_HEAT_KTCH_MODE);

    mqtt_client.subscribe(TOPIC_HEAT_BED_TEMP);
    mqtt_client.subscribe(TOPIC_HEAT_BED_HUM);
    mqtt_client.subscribe(TOPIC_HEAT_BED_TARGET);
    mqtt_client.subscribe(TOPIC_HEAT_BED_ACTION);
    mqtt_client.subscribe(TOPIC_HEAT_BED_PRESET);
    mqtt_client.subscribe(TOPIC_HEAT_BED_WINDOW);
    mqtt_client.subscribe(TOPIC_HEAT_BED_MODE);

    mqtt_client.subscribe(TOPIC_HEAT_LIVE_TEMP);
    mqtt_client.subscribe(TOPIC_HEAT_LIVE_HUM);
    mqtt_client.subscribe(TOPIC_HEAT_LIVE_TARGET);
    mqtt_client.subscribe(TOPIC_HEAT_LIVE_ACTION);
    mqtt_client.subscribe(TOPIC_HEAT_LIVE_PRESET);
    mqtt_client.subscribe(TOPIC_HEAT_LIVE_WINDOW);  // balcony_door_contact
    mqtt_client.subscribe(TOPIC_HEAT_LIVE_MODE);

    mqtt_client.subscribe(TOPIC_HEAT_ART_TEMP);
    mqtt_client.subscribe(TOPIC_HEAT_ART_HUM);
    mqtt_client.subscribe(TOPIC_HEAT_ART_TARGET);
    mqtt_client.subscribe(TOPIC_HEAT_ART_ACTION);
    mqtt_client.subscribe(TOPIC_HEAT_ART_PRESET);
    mqtt_client.subscribe(TOPIC_HEAT_ART_WINDOW);
    mqtt_client.subscribe(TOPIC_HEAT_ART_MODE);

    // ── Switches / lights ─────────────────────────────────────────────────────
    mqtt_client.subscribe(TOPIC_SW_TV_STATE);
    mqtt_client.subscribe(TOPIC_SW_TVSOCK_STATE);
    mqtt_client.subscribe(TOPIC_SW_SHIELD_STATE);
    mqtt_client.subscribe(TOPIC_SW_LIVING_STATE);
    mqtt_client.subscribe(TOPIC_SW_KITCHEN_STATE);
    mqtt_client.subscribe(TOPIC_SW_HEATLAMP_STATE);
    // Bedroom lights
    mqtt_client.subscribe(TOPIC_BED_NIGHTSTAND_STATE);
    mqtt_client.subscribe(TOPIC_BED_LIGHT_STATE);

    // ── Vacuum ────────────────────────────────────────────────────────────────
    mqtt_client.subscribe(TOPIC_VAC_ROOM_BATH);
    mqtt_client.subscribe(TOPIC_VAC_ROOM_HALL);
    mqtt_client.subscribe(TOPIC_VAC_ROOM_BED);
    mqtt_client.subscribe(TOPIC_VAC_ROOM_LIVE);
    mqtt_client.subscribe(TOPIC_VAC_ROOM_KIDS);
    mqtt_client.subscribe(TOPIC_VAC_ROOM_KTCH);
    mqtt_client.subscribe(TOPIC_VAC_REPEAT);

    // ── Climate overview ──────────────────────────────────────────────────────
    mqtt_client.subscribe(TOPIC_OUT_TEMP);
    mqtt_client.subscribe(TOPIC_OUT_HUM);
    mqtt_client.subscribe(TOPIC_CO2);
    mqtt_client.subscribe(TOPIC_PM25);

    // ── ThinkLab ──────────────────────────────────────────────────────────────
    mqtt_client.subscribe(TOPIC_TL_CPU);
    mqtt_client.subscribe(TOPIC_TL_DISK1);
    mqtt_client.subscribe(TOPIC_TL_DISK2);
    mqtt_client.subscribe(TOPIC_TL_DISK3);
    mqtt_client.subscribe(TOPIC_TL_DISK4);
    mqtt_client.subscribe(TOPIC_TL_DISK5);
    mqtt_client.subscribe(TOPIC_TL_LXC_ABS);
    mqtt_client.subscribe(TOPIC_TL_LXC_JEL);
    mqtt_client.subscribe(TOPIC_TL_LXC_PIH);
    mqtt_client.subscribe(TOPIC_TL_LXC_SMB);
    mqtt_client.subscribe(TOPIC_TL_LXC_ZT);
    mqtt_client.subscribe(TOPIC_TL_LXC_SPR);
    mqtt_client.subscribe(TOPIC_TL_VM_HAOS);

    Serial.printf("[MQTT] subscribed (%d topics)\n", 63);
}

// ─── Reconnect ────────────────────────────────────────────────────────────────
static void reconnect() {
    static uint32_t last_attempt = 0;
    if (millis() - last_attempt < 5000) return;
    last_attempt = millis();

    Serial.printf("[MQTT] connecting to %s:%d … ", MQTT_HOST, MQTT_PORT);

    bool ok = mqtt_client.connect(
        MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS,
        MQTT_AVAIL_TOPIC, /*qos*/0, /*retain*/true, "offline"
    );

    if (ok) {
        Serial.println("connected");
        mqtt_client.publish(MQTT_AVAIL_TOPIC, "online", /*retain=*/true);
        publish_discovery();
        subscribe_all();
        // Publish after subscribing so the HA automation's disk pushes
        // arrive after the device is already listening on those topics.
        mqtt_client.publish("paperdash/status", "online", /*retain=*/false);
    } else {
        Serial.printf("failed (rc=%d)\n", mqtt_client.state());
    }
}

// ─── Public API ───────────────────────────────────────────────────────────────
void mqtt_init() {
    mqtt_client.setServer(MQTT_HOST, MQTT_PORT);
    mqtt_client.setCallback(on_message);
    mqtt_client.setKeepAlive(60);
    mqtt_client.setBufferSize(1024);
}

void mqtt_loop() {
    if (!mqtt_client.connected()) reconnect();
    mqtt_client.loop();
}

void mqtt_publish(const char *topic, const char *value) {
    if (!mqtt_client.connected()) return;
    mqtt_client.publish(topic, value, false);
    Serial.printf("[MQTT] pub %s → %s\n", topic, value);
}

void mqtt_publish_float(const char *topic, float value) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%.1f", value);
    mqtt_publish(topic, buf);
}

bool mqtt_connected() { return mqtt_client.connected(); }

void mqtt_publish_status(uint8_t battery_pct, int8_t rssi) {
    if (!mqtt_client.connected()) return;
    char buf[8];
    snprintf(buf, sizeof(buf), "%u", battery_pct);
    mqtt_client.publish(MQTT_BAT_STATE,  buf, /*retain=*/true);
    snprintf(buf, sizeof(buf), "%d", rssi);
    mqtt_client.publish(MQTT_WIFI_STATE, buf, /*retain=*/true);
}
