#pragma once
#include <lvgl.h>

/** Build the vacuum/cleaning control page. Call once from ui_init(). */
void build_page_cleaning();

/** Return the cleaning page handle (used by home_page grid). */
lv_obj_t *get_page_cleaning();

/**
 * Route a binary state update to a vacuum room toggle.
 * Handles: vac_room_* input_boolean topics.
 * @return true if the topic was consumed.
 */
bool cleaning_page_update_switch(const char *topic, bool on);

/**
 * Route a sensor value to the cleaning page.
 * Handles: vacuum repeat selection topic.
 * @return true if the topic was consumed.
 */
bool cleaning_page_update_sensor(const char *topic, const char *value);
