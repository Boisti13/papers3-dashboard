/**
 * Minimal lv_conf.h for M5Stack PaperS3 — 960×540 e-paper, 16-gray
 * LVGL 8.3.x
 */
#if 1  /* Set this to "1" to enable the content */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/* Colour depth — M5GFX uses RGB565 16bpp */
#define LV_COLOR_DEPTH 16

/* Swap bytes for correct RGB565 byte order on ESP32 */
#define LV_COLOR_16_SWAP 1

/* LVGL uses PSRAM for all internal allocations */
#define LV_MEM_CUSTOM 1
#define LV_MEM_CUSTOM_INCLUDE <esp32-hal-psram.h>
#define LV_MEM_CUSTOM_ALLOC   ps_malloc
#define LV_MEM_CUSTOM_REALLOC ps_realloc
#define LV_MEM_CUSTOM_FREE    free
#define LV_MEM_SIZE (256U * 1024U)  /* fallback, not used when CUSTOM=1 */

/* Display resolution (matches EPD_WIDTH / EPD_HEIGHT in config.h) */
#define LV_HOR_RES_MAX 540
#define LV_VER_RES_MAX 960

/* DPI — 4.7" diagonal at 960×540 ≈ 234 px/inch */
#define LV_DPI_DEF 234

/* Tick source provided by esp_timer in main.cpp */
#define LV_TICK_CUSTOM 1
#define LV_TICK_CUSTOM_INCLUDE "esp_timer.h"
#define LV_TICK_CUSTOM_SYS_TIME_EXPR ((uint32_t)(esp_timer_get_time() / 1000ULL))

/* Task handler called from main loop — no OS integration needed */
#define LV_USE_OS LV_OS_NONE

/* Enable widgets we use */
#define LV_USE_BTN     1
#define LV_USE_LABEL   1
#define LV_USE_TABVIEW 1
#define LV_USE_SLIDER  1
#define LV_USE_ARC     0
#define LV_USE_CHART   0
#define LV_USE_TABLE   0
#define LV_USE_CHECKBOX 1
#define LV_USE_SWITCH  1
#define LV_USE_TEXTAREA 0
#define LV_USE_KEYBOARD 0
#define LV_USE_MSGBOX  0
#define LV_USE_SPINBOX 0
#define LV_USE_SPINNER 0
#define LV_USE_DROPDOWN 1
#define LV_USE_ROLLER  0
#define LV_USE_LIST    0
#define LV_USE_MENU    0
#define LV_USE_METER   0

/* Logging — route to Serial via lv_log_register_print_cb */
#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
#define LV_LOG_PRINTF 0

/* Font — built-in Montserrat 14 (good default) */
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_MONTSERRAT_36 1
#define LV_FONT_MONTSERRAT_48 1
#define LV_FONT_DEFAULT &lv_font_montserrat_14

/* Custom MDI icon font (lv_font_mdi_48.c) */
#define LV_FONT_CUSTOM_DECLARE  LV_FONT_DECLARE(lv_font_mdi_48) \
                                LV_FONT_DECLARE(lv_font_mdi_20)

/* Misc */
#define LV_USE_PERF_MONITOR 0
#define LV_USE_MEM_MONITOR  0
#define LV_USE_REFR_DEBUG   0
#define LV_USE_ASSERT_NULL          1
#define LV_USE_ASSERT_MALLOC        1
#define LV_USE_ASSERT_STYLE         0
#define LV_USE_ASSERT_MEM_INTEGRITY 0
#define LV_USE_ASSERT_OBJ           0

#endif /* LV_CONF_H */
#endif /* End of "Content enable" */
