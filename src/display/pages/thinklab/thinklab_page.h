#pragma once
#include <lvgl.h>

/** Build the ThinkLab resources + services page. Call once from ui_init(). */
void build_page_thinklab();

/** Return the thinklab page handle (used by home_page grid). */
lv_obj_t *get_page_thinklab();

/**
 * Route a sensor value to a ThinkLab widget (CPU, disk usage).
 * @return true if the topic was consumed.
 */
bool thinklab_page_update_sensor(const char *topic, const char *value);

/**
 * Route a binary state to a ThinkLab service row.
 * Handles: LXC/VM switch topics.
 * @return true if the topic was consumed.
 */
bool thinklab_page_update_switch(const char *topic, bool on);
