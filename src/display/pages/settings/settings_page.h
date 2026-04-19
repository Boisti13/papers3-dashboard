#pragma once
#include <lvgl.h>

/** Build the Settings stub page. Call once from ui_init(). */
void build_page_settings();

/** Return the settings page handle (used by home_page grid). */
lv_obj_t *get_page_settings();

/** Placeholder — always returns false. */
bool settings_page_update_sensor(const char *topic, const char *value);
bool settings_page_update_switch(const char *topic, bool on);
