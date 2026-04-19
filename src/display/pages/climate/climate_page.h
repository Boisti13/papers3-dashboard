#pragma once
#include <lvgl.h>

/** Build the outdoor + air quality overview page. Call once from ui_init(). */
void build_page_climate();

/** Return the climate page handle (used by home_page grid). */
lv_obj_t *get_page_climate();

/**
 * Route a sensor value to a climate widget.
 * Handles: outdoor temp/hum, CO2, PM2.5.
 * @return true if the topic was consumed.
 */
bool climate_page_update_sensor(const char *topic, const char *value);

/** Climate page has no binary switch topics — always returns false. */
bool climate_page_update_switch(const char *topic, bool on);
