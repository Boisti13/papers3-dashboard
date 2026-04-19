#pragma once
#include <lvgl.h>

/**
 * icons.h — Material Design Icons (MDI) codepoints used in the dashboard.
 *
 * Font file : src/fonts/lv_font_mdi_48.c
 * Symbol    : lv_font_mdi_48  (48 px, bpp 4)
 *
 * To regenerate:
 *   1. Download TTF:
 *      https://github.com/Templarian/MaterialDesign-Webfont/raw/master/fonts/materialdesignicons-webfont.ttf
 *   2. Open https://lvgl.io/tools/fontconverter
 *      Name: lv_font_mdi_48  |  Size: 48  |  Bpp: 4
 *      Range (decimal, comma-separated):
 *      983608,984120,985815,984335,984462,984497,984855,983850,984249,984163,
 *      984845,984074,984036,987036,985504,985712,983779,988525,985692,985116,
 *      984322,985063,984602,983861,989765,986848,983754,984203,985721
 *   3. Save as: src/fonts/materialdesignicons-webfont.c
 *   4. Fix identifier (LVGL converter uses filename as symbol name):
 *      sed -i 's/materialdesignicons-webfont/lv_font_mdi_48/g; \
 *              s/MATERIALDESIGNICONS-WEBFONT/LV_FONT_MDI_48/g' \
 *              src/fonts/materialdesignicons-webfont.c
 *   5. Also remove the line ".static_bitmap = 0," if it appears near the end.
 *
 * lv_conf.h must contain:
 *   #define LV_FONT_CUSTOM_DECLARE  LV_FONT_DECLARE(lv_font_mdi_48)
 *
 * Usage:
 *   lv_obj_set_style_text_font(lbl, &lv_font_mdi_48, 0);
 *   lv_label_set_text(lbl, MDI_FIRE);
 */

LV_FONT_DECLARE(lv_font_mdi_48);

// ── UTF-8 encoding note ───────────────────────────────────────────────────────
// All MDI codepoints are in the supplementary PUA (U+F0000–U+FFFFF, plane 15).
// They encode as 4-byte UTF-8:  \xf3\xb0\xHH\xHH  (or \xf3\xb1\xHH\xHH for
// codepoints above U+F0FFF).
//
// Formula for U+F0000 + N:
//   byte1 = 0xF3
//   byte2 = 0xB0 | (N >> 12)
//   byte3 = 0x80 | ((N >> 6) & 0x3F)
//   byte4 = 0x80 | (N & 0x3F)

// ── Heating ───────────────────────────────────────────────────────────────────
#define MDI_FIRE               "\xf3\xb0\x88\xb8"  // U+F0238  mdi:fire
#define MDI_RADIATOR           "\xf3\xb0\x90\xb8"  // U+F0438  mdi:radiator
#define MDI_RADIATOR_DISABLED  "\xf3\xb0\xab\x97"  // U+F0AD7  mdi:radiator-disabled
#define MDI_THERMOMETER        "\xf3\xb0\x94\x8f"  // U+F050F  mdi:thermometer
#define MDI_WATER_PERCENT      "\xf3\xb0\x96\x8e"  // U+F058E  mdi:water-percent
#define MDI_WINDOW_OPEN        "\xf3\xb0\x96\xb1"  // U+F05B1  mdi:window-open
#define MDI_SNOWFLAKE          "\xf3\xb0\x9c\x97"  // U+F0717  mdi:snowflake
#define MDI_LEAF               "\xf3\xb0\x8c\xaa"  // U+F032A  mdi:leaf
#define MDI_SOFA               "\xf3\xb0\x92\xb9"  // U+F04B9  mdi:sofa
#define MDI_ROCKET             "\xf3\xb0\x91\xa3"  // U+F0463  mdi:rocket

// ── Vacuum / Cleaning ─────────────────────────────────────────────────────────
#define MDI_ROBOT_VACUUM       "\xf3\xb0\x9c\x8d"  // U+F070D  mdi:robot-vacuum
#define MDI_PLAY               "\xf3\xb0\x90\x8a"  // U+F040A  mdi:play
#define MDI_PAUSE              "\xf3\xb0\x8f\xa4"  // U+F03E4  mdi:pause
#define MDI_HOME_IMPORT        "\xf3\xb0\xbe\x9c"  // U+F0F9C  mdi:home-import-outline
#define MDI_TRASH_CAN          "\xf3\xb0\xa9\xb9"  // U+F0A79  mdi:trash-can-outline

// ── Rooms ─────────────────────────────────────────────────────────────────────
#define MDI_SHOWER             "\xf3\xb0\xa6\xa0"  // U+F09A0  mdi:shower
#define MDI_SILVERWARE_FORK    "\xf3\xb0\xa9\xb0"  // U+F0A70  mdi:silverware-fork-knife
#define MDI_BED                "\xf3\xb0\x8b\xa3"  // U+F02E3  mdi:bed
#define MDI_SOFA_OUTLINE       "\xf3\xb1\x95\xad"  // U+F156D  mdi:sofa-outline
#define MDI_HUMAN_MALE_BOY     "\xf3\xb0\xa9\x9c"  // U+F0A5C  mdi:human-male-boy
#define MDI_DOOR_OPEN          "\xf3\xb0\xa0\x9c"  // U+F081C  mdi:door-open

// ── Switches ──────────────────────────────────────────────────────────────────
#define MDI_TELEVISION         "\xf3\xb0\x94\x82"  // U+F0502  mdi:television
#define MDI_POWER_SOCKET       "\xf3\xb0\x9f\xa7"  // U+F07E7  mdi:power-socket-eu
#define MDI_NVIDIA             "\xf3\xb0\x98\x9a"  // U+F061A  mdi:chip  (nvidia removed from MDI)
#define MDI_LIGHTBULB          "\xf3\xb0\x8c\xb5"  // U+F0335  mdi:lightbulb
#define MDI_HEAT_WAVE          "\xf3\xb1\xa9\x85"  // U+F1A45  mdi:heat-wave

// ── ThinkLab ──────────────────────────────────────────────────────────────────
#define MDI_CPU_64_BIT         "\xf3\xb0\xbb\xa0"  // U+F0EE0  mdi:cpu-64-bit
#define MDI_HARDDISK           "\xf3\xb0\x8b\x8a"  // U+F02CA  mdi:harddisk
#define MDI_SERVER             "\xf3\xb0\x92\x8b"  // U+F048B  mdi:server
