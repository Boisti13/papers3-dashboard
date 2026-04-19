#include "nav.h"
#include "styles.h"
#include "epd_driver.h"
#include "../config.h"

// ── Internal state ────────────────────────────────────────────────────────────
static lv_obj_t  *s_content       = nullptr;
static lv_obj_t  *s_lbl_page_name = nullptr;
static lv_obj_t  *s_btn_back      = nullptr;
static lv_obj_t  *s_home_page     = nullptr;
static lv_obj_t  *s_cur_page      = nullptr;
static const char *s_cur_page_name = "Home";

#define NAV_STACK_SIZE 4
static lv_obj_t  *s_nav_stack_pages[NAV_STACK_SIZE];
static const char *s_nav_stack_names[NAV_STACK_SIZE];
static int         s_nav_depth = 0;

// ── Public init ───────────────────────────────────────────────────────────────
void nav_init(lv_obj_t *content, lv_obj_t *lbl_page_name, lv_obj_t *btn_back) {
    s_content       = content;
    s_lbl_page_name = lbl_page_name;
    s_btn_back      = btn_back;
    s_nav_depth     = 0;
}

void nav_set_home(lv_obj_t *home_page) {
    s_home_page = home_page;
    s_cur_page  = home_page;
}

// ── Navigation ────────────────────────────────────────────────────────────────
void navigate_to(lv_obj_t *page, const char *name) {
    if (s_cur_page && s_cur_page != page) {
        lv_obj_add_flag(s_cur_page, LV_OBJ_FLAG_HIDDEN);
        if (s_nav_depth < NAV_STACK_SIZE) {
            s_nav_stack_pages[s_nav_depth] = s_cur_page;
            s_nav_stack_names[s_nav_depth] = s_cur_page_name;
            s_nav_depth++;
        }
    }
    lv_obj_clear_flag(page, LV_OBJ_FLAG_HIDDEN);
    s_cur_page      = page;
    s_cur_page_name = name;
    if (s_lbl_page_name) lv_label_set_text(s_lbl_page_name, name);
    if (s_btn_back) {
        if (s_nav_depth > 0)
            lv_obj_clear_flag(s_btn_back, LV_OBJ_FLAG_HIDDEN);
        else
            lv_obj_add_flag(s_btn_back, LV_OBJ_FLAG_HIDDEN);
    }
    lv_refr_now(lv_disp_get_default());
    epd_driver_full_refresh();
}

void navigate_back() {
    if (s_nav_depth == 0) return;
    if (s_cur_page) lv_obj_add_flag(s_cur_page, LV_OBJ_FLAG_HIDDEN);
    s_nav_depth--;
    s_cur_page      = s_nav_stack_pages[s_nav_depth];
    s_cur_page_name = s_nav_stack_names[s_nav_depth];
    lv_obj_clear_flag(s_cur_page, LV_OBJ_FLAG_HIDDEN);
    if (s_lbl_page_name) lv_label_set_text(s_lbl_page_name, s_cur_page_name);
    if (s_btn_back && s_nav_depth == 0)
        lv_obj_add_flag(s_btn_back, LV_OBJ_FLAG_HIDDEN);
    lv_refr_now(lv_disp_get_default());
    epd_driver_full_refresh();
}

void navigate_home() {
    if (s_cur_page && s_cur_page != s_home_page)
        lv_obj_add_flag(s_cur_page, LV_OBJ_FLAG_HIDDEN);
    s_nav_depth     = 0;
    s_cur_page      = s_home_page;
    s_cur_page_name = "Home";
    if (s_home_page) lv_obj_clear_flag(s_home_page, LV_OBJ_FLAG_HIDDEN);
    if (s_lbl_page_name) lv_label_set_text(s_lbl_page_name, "Home");
    if (s_btn_back) lv_obj_add_flag(s_btn_back, LV_OBJ_FLAG_HIDDEN);
    lv_refr_now(lv_disp_get_default());
    epd_driver_full_refresh();
}

// ── Page factory ──────────────────────────────────────────────────────────────
lv_obj_t *make_page() {
    lv_obj_t *p = lv_obj_create(s_content);
    lv_obj_remove_style_all(p);
    lv_obj_add_style(p, &g_sty_page, 0);
    lv_obj_set_size(p, EPD_WIDTH, CONTENT_H);
    lv_obj_set_pos(p, 0, 0);
    lv_obj_clear_flag(p, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(p, LV_OBJ_FLAG_HIDDEN);
    return p;
}

// ── Label helpers ─────────────────────────────────────────────────────────────
lv_obj_t *make_info_label(lv_obj_t *parent, int32_t x, int32_t y, const char *text) {
    lv_obj_t *lbl = lv_label_create(parent);
    lv_obj_add_style(lbl, &g_sty_lbl_md, 0);
    lv_label_set_text(lbl, text);
    lv_obj_set_pos(lbl, x, y);
    return lbl;
}

lv_obj_t *make_value_label(lv_obj_t *parent, int32_t x, int32_t y, const char *text) {
    lv_obj_t *lbl = lv_label_create(parent);
    lv_obj_add_style(lbl, &g_sty_lbl_lg, 0);
    lv_label_set_text(lbl, text);
    lv_obj_set_pos(lbl, x, y);
    return lbl;
}
