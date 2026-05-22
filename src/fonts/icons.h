#pragma once
#include <lvgl.h>

/**
 * icons.h — Material Design Icons (MDI) codepoints used in the dashboard.
 *
 * Font file : src/fonts/lv_font_mdi_48.c
 * Symbol    : lv_font_mdi_48  (48 px, bpp 4)
 *
 * To regenerate lv_font_mdi_48:
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
 * To regenerate lv_font_mdi_20 (WiFi + battery header icons):
 *   Same steps but:
 *      Name: lv_font_mdi_20  |  Size: 20  |  Bpp: 4
 *      Range (decimal, comma-separated):
 *      985375,985378,985381,985384,985390,
 *      983161,983162,983163,983164,983165,983166,983167,983168,983169,983170,983171,
 *      983172,983173,983174,983175,983176,983177,983178,983179,985244,985245,985246,987341
 *      (WiFi: 985375–985390; battery non-chg: 983161–983171; battery chg: 983172–983179,985244–985246; empty alert: 987341)
 *   Save as: src/fonts/lv_font_mdi_20.c
 *
 * lv_conf.h must contain:
 *   #define LV_FONT_CUSTOM_DECLARE  LV_FONT_DECLARE(lv_font_mdi_48)
 *
 * Usage:
 *   lv_obj_set_style_text_font(lbl, &lv_font_mdi_48, 0);
 *   lv_label_set_text(lbl, MDI_FIRE);
 */

LV_FONT_DECLARE(lv_font_mdi_48);
LV_FONT_DECLARE(lv_font_mdi_20);
LV_FONT_DECLARE(lv_font_mdi_battery_20);

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

// ── WiFi signal (lv_font_mdi_20) ─────────────────────────────────────────────
#define MDI_WIFI_1             "\xf3\xb0\xa4\x9f"  // U+F091F  mdi:wifi-strength-1
#define MDI_WIFI_2             "\xf3\xb0\xa4\xa2"  // U+F0922  mdi:wifi-strength-2
#define MDI_WIFI_3             "\xf3\xb0\xa4\xa5"  // U+F0925  mdi:wifi-strength-3
#define MDI_WIFI_4             "\xf3\xb0\xa4\xa8"  // U+F0928  mdi:wifi-strength-4
#define MDI_WIFI_OFF           "\xf3\xb0\xa4\xae"  // U+F092E  mdi:wifi-strength-off-outline

// ── Battery — not charging (lv_font_mdi_20) ───────────────────────────────────
#define MDI_BAT_EMPTY          "\xf3\xb1\x83\x8d"  // U+F10CD  mdi:battery-alert (0 %)
#define MDI_BAT_0              "\xf3\xb0\x81\xb9"  // U+F0079  mdi:battery-outline (100 %)
#define MDI_BAT_10             "\xf3\xb0\x81\xba"  // U+F007A  mdi:battery-10
#define MDI_BAT_20             "\xf3\xb0\x81\xbb"  // U+F007B  mdi:battery-20
#define MDI_BAT_30             "\xf3\xb0\x81\xbc"  // U+F007C  mdi:battery-30
#define MDI_BAT_40             "\xf3\xb0\x81\xbd"  // U+F007D  mdi:battery-40
#define MDI_BAT_50             "\xf3\xb0\x81\xbe"  // U+F007E  mdi:battery-50
#define MDI_BAT_60             "\xf3\xb0\x81\xbf"  // U+F007F  mdi:battery-60
#define MDI_BAT_70             "\xf3\xb0\x82\x80"  // U+F0080  mdi:battery-70
#define MDI_BAT_80             "\xf3\xb0\x82\x81"  // U+F0081  mdi:battery-80
#define MDI_BAT_90             "\xf3\xb0\x82\x82"  // U+F0082  mdi:battery-90
#define MDI_BAT_100            "\xf3\xb0\x82\x83"  // U+F0083  mdi:battery

// ── Battery — charging (lv_font_mdi_20) ──────────────────────────────────────
#define MDI_BAT_CHG_0          "\xf3\xb0\xa2\x9c"  // U+F089C  mdi:battery-charging-outline
#define MDI_BAT_CHG_10         "\xf3\xb0\x82\x86"  // U+F0086  mdi:battery-charging-10
#define MDI_BAT_CHG_20         "\xf3\xb0\x82\x87"  // U+F0087  mdi:battery-charging-20
#define MDI_BAT_CHG_30         "\xf3\xb0\x82\x88"  // U+F0088  mdi:battery-charging-30
#define MDI_BAT_CHG_50         "\xf3\xb0\xa2\x9d"  // U+F089D  mdi:battery-charging-50
#define MDI_BAT_CHG_60         "\xf3\xb0\x82\x89"  // U+F0089  mdi:battery-charging-60
#define MDI_BAT_CHG_70         "\xf3\xb0\xa2\x9e"  // U+F089E  mdi:battery-charging-70
#define MDI_BAT_CHG_80         "\xf3\xb0\x82\x8a"  // U+F008A  mdi:battery-charging-80
#define MDI_BAT_CHG_90         "\xf3\xb0\x82\x8b"  // U+F008B  mdi:battery-charging-90
#define MDI_BAT_CHG_FULL       "\xf3\xb0\x82\x84"  // U+F0084  mdi:battery-charging-outline (100% on USB)
#define MDI_BAT_CHG_100        "\xf3\xb0\x82\x85"  // U+F0085  mdi:battery-charging

// ── ThinkLab ──────────────────────────────────────────────────────────────────
#define MDI_CPU_64_BIT         "\xf3\xb0\xbb\xa0"  // U+F0EE0  mdi:cpu-64-bit
#define MDI_HARDDISK           "\xf3\xb0\x8b\x8a"  // U+F02CA  mdi:harddisk
#define MDI_SERVER             "\xf3\xb0\x92\x8b"  // U+F048B  mdi:server
