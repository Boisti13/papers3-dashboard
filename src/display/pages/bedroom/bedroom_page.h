#pragma once
#include <lvgl.h>

/** Build the Bedroom page. Call once from ui_init(). */
void build_page_bedroom();

/** Return the bedroom page handle (used by home_page grid). */
lv_obj_t *get_page_bedroom();

/** Placeholder — always returns false. */
bool bedroom_page_update_sensor(const char *topic, const char *value);
bool bedroom_page_update_switch(const char *topic, bool on);
