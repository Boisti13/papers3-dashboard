#pragma once
#include <lvgl.h>

/** Build the heating list page and all 5 room sub-pages. Call once from ui_init(). */
void build_page_heating();

/** Return the heating list page handle (used by home_page grid). */
lv_obj_t *get_page_heating();

/**
 * Route an MQTT sensor value to a heating room widget.
 * Handles: temp, humidity, target setpoint, hvac_action, preset_mode, window contact.
 * @return true if the topic was consumed by this module.
 */
bool heating_page_update_sensor(const char *topic, const char *value);

/**
 * Heating page has no binary switch topics — always returns false.
 * Provided for interface uniformity.
 */
bool heating_page_update_switch(const char *topic, bool on);
