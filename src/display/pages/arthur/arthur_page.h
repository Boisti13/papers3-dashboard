#pragma once
#include <lvgl.h>

/** Build the Arthur stub page. Call once from ui_init(). */
void build_page_arthur();

/** Return the arthur page handle (used by home_page grid). */
lv_obj_t *get_page_arthur();

/** Placeholder — always returns false. */
bool arthur_page_update_sensor(const char *topic, const char *value);
bool arthur_page_update_switch(const char *topic, bool on);
