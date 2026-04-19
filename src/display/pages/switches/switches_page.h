#pragma once
#include <lvgl.h>

/** Build the 2×3 switch tiles page. Call once from ui_init(). */
void build_page_switches();

/** Return the switches page handle (used by home_page grid). */
lv_obj_t *get_page_switches();

/**
 * Route a binary state update to a switch tile.
 * @return true if the topic was consumed.
 */
bool switches_page_update_switch(const char *topic, bool on);

/** Switches page has no sensor topics — always returns false. */
bool switches_page_update_sensor(const char *topic, const char *value);
