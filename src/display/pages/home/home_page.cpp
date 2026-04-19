#include "home_page.h"
#include "../../styles.h"
#include "../../nav.h"
#include "../../../config.h"
#include "../heating/heating_page.h"
#include "../switches/switches_page.h"
#include "../cleaning/cleaning_page.h"
#include "../climate/climate_page.h"
#include "../thinklab/thinklab_page.h"
#include "../arthur/arthur_page.h"
#include "../bedroom/bedroom_page.h"
#include "../settings/settings_page.h"

// ── Layout ────────────────────────────────────────────────────────────────────
#define GRID_COLS   2
#define GRID_ROWS   4
#define GRID_BTN_W  ((EPD_WIDTH  - PAD * 2 - GAP * (GRID_COLS - 1)) / GRID_COLS)
#define GRID_BTN_H  ((CONTENT_H  - PAD * 2 - GAP * (GRID_ROWS - 1)) / GRID_ROWS)

static lv_obj_t *s_page_home = nullptr;

// ── ThinkLab confirm dialog ───────────────────────────────────────────────────
static lv_obj_t  *s_modal     = nullptr;
static lv_timer_t *s_modal_tmr = nullptr;

static void modal_close() {
    if (s_modal_tmr) {
        lv_timer_del(s_modal_tmr);
        s_modal_tmr = nullptr;
    }
    if (s_modal) {
        lv_obj_del(s_modal);
        s_modal = nullptr;
    }
}

static void cb_modal_timeout(lv_timer_t *t) {
    (void)t;
    modal_close();
    navigate_home();
}

static void cb_confirm_yes(lv_event_t *e) {
    (void)e;
    modal_close();
    navigate_to(get_page_thinklab(), "ThinkLab");
}

static void cb_confirm_no(lv_event_t *e) {
    (void)e;
    modal_close();
}

static void show_thinklab_confirm() {
    s_modal = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(s_modal);
    lv_obj_set_size(s_modal, EPD_WIDTH, EPD_HEIGHT);
    lv_obj_set_pos(s_modal, 0, 0);
    lv_obj_set_style_bg_color(s_modal, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(s_modal, LV_OPA_30, 0);
    lv_obj_clear_flag(s_modal, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(s_modal, LV_OBJ_FLAG_CLICKABLE);

    const int32_t dlg_w = 400;
    const int32_t dlg_h = 220;
    lv_obj_t *dlg = lv_obj_create(s_modal);
    lv_obj_remove_style_all(dlg);
    lv_obj_add_style(dlg, &g_sty_card, 0);
    lv_obj_set_style_border_width(dlg, 3, 0);
    lv_obj_set_size(dlg, dlg_w, dlg_h);
    lv_obj_align(dlg, LV_ALIGN_CENTER, 0, 0);
    lv_obj_clear_flag(dlg, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl = lv_label_create(dlg);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(lbl, lv_color_black(), 0);
    lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lbl, "Enter ThinkLab?");
    lv_obj_set_width(lbl, dlg_w - PAD * 2);
    lv_obj_align(lbl, LV_ALIGN_TOP_MID, 0, PAD);

    const int32_t btn_w = 140;
    const int32_t btn_h = 60;

    lv_obj_t *btn_yes = lv_btn_create(dlg);
    lv_obj_remove_style_all(btn_yes);
    lv_obj_add_style(btn_yes, &g_sty_btn, 0);
    lv_obj_set_size(btn_yes, btn_w, btn_h);
    lv_obj_align(btn_yes, LV_ALIGN_BOTTOM_LEFT, PAD, -PAD);
    lv_obj_add_event_cb(btn_yes, cb_confirm_yes, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *lbl_yes = lv_label_create(btn_yes);
    lv_obj_set_style_text_font(lbl_yes, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(lbl_yes, lv_color_black(), 0);
    lv_label_set_text(lbl_yes, "Yes");
    lv_obj_center(lbl_yes);

    lv_obj_t *btn_no = lv_btn_create(dlg);
    lv_obj_remove_style_all(btn_no);
    lv_obj_add_style(btn_no, &g_sty_btn, 0);
    lv_obj_set_size(btn_no, btn_w, btn_h);
    lv_obj_align(btn_no, LV_ALIGN_BOTTOM_RIGHT, -PAD, -PAD);
    lv_obj_add_event_cb(btn_no, cb_confirm_no, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *lbl_no = lv_label_create(btn_no);
    lv_obj_set_style_text_font(lbl_no, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(lbl_no, lv_color_black(), 0);
    lv_label_set_text(lbl_no, "No");
    lv_obj_center(lbl_no);

    s_modal_tmr = lv_timer_create(cb_modal_timeout, 10000, nullptr);
    lv_timer_set_repeat_count(s_modal_tmr, 1);
}

// ── Grid ──────────────────────────────────────────────────────────────────────
struct GridItem {
    const char *label;
    lv_obj_t *(*get_page)();
    const char *name;
};

static void cb_grid_btn(lv_event_t *e) {
    GridItem *item = (GridItem *)lv_event_get_user_data(e);
    if (!item || !item->get_page) return;
    if (item->get_page == get_page_thinklab) {
        show_thinklab_confirm();
    } else {
        navigate_to(item->get_page(), item->name);
    }
}

void build_page_home() {
    s_page_home = make_page();

    static GridItem grid[GRID_ROWS * GRID_COLS] = {
        { "Heating",  get_page_heating,  "Heating"  },
        { "Switches", get_page_switches, "Switches" },
        { "Cleaning", get_page_cleaning, "Cleaning" },
        { "Climate",  get_page_climate,  "Climate"  },
        { "ThinkLab", get_page_thinklab, "ThinkLab" },
        { "Arthur",   get_page_arthur,   "Arthur"   },
        { "Bedroom",  get_page_bedroom,  "Bedroom"  },
        { "",         nullptr,           ""         },
    };

    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            int     idx = row * GRID_COLS + col;
            int32_t x   = col * (GRID_BTN_W + GAP);
            int32_t y   = row * (GRID_BTN_H + GAP);

            lv_obj_t *btn = lv_btn_create(s_page_home);
            lv_obj_remove_style_all(btn);
            lv_obj_add_style(btn, &g_sty_btn_grid, 0);
            lv_obj_set_size(btn, GRID_BTN_W, GRID_BTN_H);
            lv_obj_set_pos(btn, x, y);

            lv_obj_t *lbl = lv_label_create(btn);
            lv_obj_add_style(lbl, &g_sty_lbl_lg, 0);
            lv_label_set_text(lbl, grid[idx].label[0] ? grid[idx].label : "-");
            lv_obj_center(lbl);

            if (grid[idx].get_page) {
                lv_obj_add_event_cb(btn, cb_grid_btn, LV_EVENT_CLICKED, &grid[idx]);
            } else {
                lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_set_style_border_color(btn, lv_color_make(0xAA, 0xAA, 0xAA), 0);
                lv_obj_set_style_text_color(lbl, lv_color_make(0xAA, 0xAA, 0xAA), 0);
            }
        }
    }
}

lv_obj_t *get_page_home() { return s_page_home; }
