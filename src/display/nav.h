#pragma once
#include <lvgl.h>
#include "../config.h"

// ── Layout constants ──────────────────────────────────────────────────────────
static const int32_t HDR_H    = 70;
static const int32_t FTR_H    = 70;
static const int32_t CONTENT_H = EPD_HEIGHT - HDR_H - FTR_H;  // 820
static const int32_t PAD      = 16;
static const int32_t GAP      = 12;

// ── Navigation init ───────────────────────────────────────────────────────────
/**
 * Call once from ui_init() after the content container and footer widgets exist.
 * @param content       The lv_obj_t that is parent of all pages.
 * @param lbl_page_name The header label showing the current page name.
 * @param btn_back      The footer Back button (hidden when at home depth).
 */
void nav_init(lv_obj_t *content, lv_obj_t *lbl_page_name, lv_obj_t *btn_back);

/**
 * Store the home page handle so navigate_home() can always return to it
 * without the caller passing a pointer every time.
 */
void nav_set_home(lv_obj_t *home_page);

// ── Navigation actions ────────────────────────────────────────────────────────
void navigate_to(lv_obj_t *page, const char *name);
void navigate_back();
void navigate_home();

// ── Page factory ──────────────────────────────────────────────────────────────
/**
 * Create a hidden child of the content container, styled as a full-size page.
 * Every page module calls this once inside its build_page_*() function.
 */
lv_obj_t *make_page();

// ── Label helpers ─────────────────────────────────────────────────────────────
/** Create a 20pt (md) info label at absolute (x,y) inside parent. */
lv_obj_t *make_info_label(lv_obj_t *parent, int32_t x, int32_t y, const char *text);

/** Create a 28pt (lg) value label at absolute (x,y) inside parent. */
lv_obj_t *make_value_label(lv_obj_t *parent, int32_t x, int32_t y, const char *text);
