#pragma once
#include <lvgl.h>

/**
 * Build the 2×4 home grid page.
 * Must be called AFTER all other pages are built so their handles are available.
 */
void build_page_home();

/** Return the home page handle so nav_set_home() can register it. */
lv_obj_t *get_page_home();
