#include "cleaning_page.h"
#include "../../styles.h"
#include "../../nav.h"
#include "../../../config.h"
#include "../../../mqtt/ha_mqtt.h"
#include "../../../fonts/icons.h"

#include <string.h>
#include <stdio.h>

// ── VacRoom struct ────────────────────────────────────────────────────────────
struct VacRoom {
    const char *name;
    const char *icon;
    const char *topic;
    lv_obj_t   *btn;
    bool        on;
};

static VacRoom s_vac_rooms[6] = {
    { "Bathroom",   MDI_SHOWER,          TOPIC_VAC_ROOM_BATH, nullptr, false },
    { "Hallway",    MDI_DOOR_OPEN,        TOPIC_VAC_ROOM_HALL, nullptr, false },
    { "Bedroom",    MDI_BED,             TOPIC_VAC_ROOM_BED,  nullptr, false },
    { "Livingroom", MDI_SOFA_OUTLINE,    TOPIC_VAC_ROOM_LIVE, nullptr, false },
    { "Arthur",     MDI_HUMAN_MALE_BOY,  TOPIC_VAC_ROOM_KIDS, nullptr, false },
    { "Kitchen",    MDI_SILVERWARE_FORK, TOPIC_VAC_ROOM_KTCH, nullptr, false },
};

static lv_obj_t *s_page_cleaning  = nullptr;
static lv_obj_t *s_btn_rep1       = nullptr;
static lv_obj_t *s_btn_rep2       = nullptr;
static lv_obj_t *s_btn_sel        = nullptr;
static char      s_vac_repeat[4]  = "1";

// ── Style helpers ─────────────────────────────────────────────────────────────
static void apply_vac_room_style(VacRoom *vr) {
    if (!vr->btn) return;
    lv_obj_set_style_border_width(vr->btn, vr->on ? 4 : 2, 0);
    lv_obj_set_style_bg_color(vr->btn,
        vr->on ? lv_color_make(0xDD, 0xDD, 0xDD) : lv_color_white(), 0);
}

static void apply_repeat_style() {
    bool rep1 = strcmp(s_vac_repeat, "1") == 0;
    if (s_btn_rep1) {
        lv_obj_set_style_border_width(s_btn_rep1, rep1 ? 4 : 2, 0);
        lv_obj_set_style_bg_color(s_btn_rep1,
            rep1 ? lv_color_make(0xDD, 0xDD, 0xDD) : lv_color_white(), 0);
    }
    if (s_btn_rep2) {
        lv_obj_set_style_border_width(s_btn_rep2, !rep1 ? 4 : 2, 0);
        lv_obj_set_style_bg_color(s_btn_rep2,
            !rep1 ? lv_color_make(0xDD, 0xDD, 0xDD) : lv_color_white(), 0);
    }
}

// ── Callbacks ─────────────────────────────────────────────────────────────────
static void cb_vac_room(lv_event_t *e) {
    VacRoom *vr = (VacRoom *)lv_event_get_user_data(e);
    vr->on = !vr->on;
    apply_vac_room_style(vr);
    // TOPIC_VAC_ROOM_* ends in "/state" (statestream path).
    // Command topic drops "/state" → "homeassistant/input_boolean/<id>/set"
    char cmd[128];
    strlcpy(cmd, vr->topic, sizeof(cmd));
    char *suffix = strstr(cmd, "/state");
    if (suffix) strcpy(suffix, "/set");
    else        strlcat(cmd, "/set", sizeof(cmd));
    mqtt_publish(cmd, vr->on ? "ON" : "OFF");
}

static void cb_rep1(lv_event_t *e) {
    strlcpy(s_vac_repeat, "1", sizeof(s_vac_repeat));
    apply_repeat_style();
    mqtt_publish(TOPIC_VAC_REPEAT_SET, "1");
}

static void cb_rep2(lv_event_t *e) {
    strlcpy(s_vac_repeat, "2", sizeof(s_vac_repeat));
    apply_repeat_style();
    mqtt_publish(TOPIC_VAC_REPEAT_SET, "2");
}

static void cb_vac_all(lv_event_t *e)   { mqtt_publish(TOPIC_VAC_START_ALL, "start"); }
static void cb_vac_pause(lv_event_t *e) { mqtt_publish(TOPIC_VAC_PAUSE,     "start"); }
static void cb_vac_dock(lv_event_t *e)  { mqtt_publish(TOPIC_VAC_DOCK,      "start"); }
static void cb_vac_trash(lv_event_t *e) { mqtt_publish(TOPIC_VAC_TRASH,     "start"); }

// Restore Start Selected button to its normal active style after the flash
static void cb_sel_flash_restore(lv_timer_t *t) {
    if (s_btn_sel) {
        lv_obj_set_style_bg_color(s_btn_sel,  lv_color_make(0xF2, 0xF2, 0xF2), 0);
        lv_obj_set_style_text_color(s_btn_sel, lv_color_black(), 0);
        // propagate colour to the label child
        lv_obj_t *lbl = lv_obj_get_child(s_btn_sel, 0);
        if (lbl) lv_obj_set_style_text_color(lbl, lv_color_black(), 0);
    }
    lv_timer_del(t);
}

static void cb_vac_sel(lv_event_t *e) {
    mqtt_publish(TOPIC_VAC_START_SEL, "start");
    // Visual feedback: invert button colours briefly
    if (s_btn_sel) {
        lv_obj_set_style_bg_color(s_btn_sel, lv_color_black(), 0);
        lv_obj_t *lbl = lv_obj_get_child(s_btn_sel, 0);
        if (lbl) lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
        lv_timer_create(cb_sel_flash_restore, 1500, nullptr);
    }
}

// ── Helper: icon+label button ─────────────────────────────────────────────────
static lv_obj_t *make_icon_btn(lv_obj_t *parent, int32_t x, int32_t y,
                                int32_t w, int32_t h,
                                const char *icon, const char *label,
                                lv_event_cb_t cb, void *user_data) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_remove_style_all(btn);
    lv_obj_add_style(btn, &g_sty_btn, 0);
    lv_obj_set_size(btn, w, h);
    lv_obj_set_pos(btn, x, y);
    if (cb) lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, user_data);

    lv_obj_t *ico = lv_label_create(btn);
    lv_obj_set_style_text_font(ico, &lv_font_mdi_48, 0);
    lv_obj_set_style_text_color(ico, lv_color_black(), 0);
    lv_label_set_text(ico, icon);
    lv_obj_align(ico, LV_ALIGN_CENTER, 0, -16);

    lv_obj_t *lbl = lv_label_create(btn);
    lv_obj_add_style(lbl, &g_sty_lbl_md, 0);
    lv_label_set_text(lbl, label);
    lv_obj_align(lbl, LV_ALIGN_BOTTOM_MID, 0, -4);

    return btn;
}

// ── Page builder ──────────────────────────────────────────────────────────────
void build_page_cleaning() {
    s_page_cleaning = make_page();

    const int32_t inner_w = EPD_WIDTH - PAD * 2;  // 508

    // ── Room tiles 3×2 ────────────────────────────────────────────────────────
    const int32_t rt_cols = 3;
    const int32_t rt_gap  = GAP;
    const int32_t rt_w    = (inner_w - rt_gap * (rt_cols - 1)) / rt_cols;  // 161
    const int32_t rt_h    = 100;

    for (int i = 0; i < 6; i++) {
        int     row = i / rt_cols;
        int     col = i % rt_cols;
        int32_t x   = col * (rt_w + rt_gap);
        int32_t y   = row * (rt_h + rt_gap);

        lv_obj_t *btn = lv_btn_create(s_page_cleaning);
        lv_obj_remove_style_all(btn);
        lv_obj_add_style(btn, &g_sty_btn, 0);
        lv_obj_set_size(btn, rt_w, rt_h);
        lv_obj_set_pos(btn, x, y);
        lv_obj_set_style_radius(btn, 10, 0);
        lv_obj_add_event_cb(btn, cb_vac_room, LV_EVENT_CLICKED, &s_vac_rooms[i]);
        s_vac_rooms[i].btn = btn;

        // MDI room icon (centered upper area)
        lv_obj_t *ico = lv_label_create(btn);
        lv_obj_set_style_text_font(ico, &lv_font_mdi_48, 0);
        lv_obj_set_style_text_color(ico, lv_color_black(), 0);
        lv_label_set_text(ico, s_vac_rooms[i].icon);
        lv_obj_align(ico, LV_ALIGN_CENTER, 0, -12);

        // Name label (bottom-centre)
        lv_obj_t *lbl = lv_label_create(btn);
        lv_obj_add_style(lbl, &g_sty_lbl_md, 0);
        lv_label_set_text(lbl, s_vac_rooms[i].name);
        lv_obj_align(lbl, LV_ALIGN_BOTTOM_MID, 0, -6);

        // Apply state that may have arrived via retained MQTT before page built
        apply_vac_room_style(&s_vac_rooms[i]);
    }

    // ── Repeat + Start Selected ───────────────────────────────────────────────
    const int32_t frame2_y = 2 * (rt_h + rt_gap) + GAP;
    const int32_t rep_h    = 76;
    const int32_t rep_w    = 80;

    s_btn_rep1 = lv_btn_create(s_page_cleaning);
    lv_obj_remove_style_all(s_btn_rep1);
    lv_obj_add_style(s_btn_rep1, &g_sty_btn, 0);
    lv_obj_set_size(s_btn_rep1, rep_w, rep_h);
    lv_obj_set_pos(s_btn_rep1, 0, frame2_y);
    lv_obj_add_event_cb(s_btn_rep1, cb_rep1, LV_EVENT_CLICKED, nullptr);
    {
        lv_obj_t *l = lv_label_create(s_btn_rep1);
        lv_obj_add_style(l, &g_sty_lbl_lg, 0);
        lv_label_set_text(l, "1x");
        lv_obj_center(l);
    }

    s_btn_rep2 = lv_btn_create(s_page_cleaning);
    lv_obj_remove_style_all(s_btn_rep2);
    lv_obj_add_style(s_btn_rep2, &g_sty_btn, 0);
    lv_obj_set_size(s_btn_rep2, rep_w, rep_h);
    lv_obj_set_pos(s_btn_rep2, rep_w + GAP, frame2_y);
    lv_obj_add_event_cb(s_btn_rep2, cb_rep2, LV_EVENT_CLICKED, nullptr);
    {
        lv_obj_t *l = lv_label_create(s_btn_rep2);
        lv_obj_add_style(l, &g_sty_lbl_lg, 0);
        lv_label_set_text(l, "2x");
        lv_obj_center(l);
    }

    const int32_t sel_x = rep_w * 2 + GAP * 3;
    const int32_t sel_w = inner_w - sel_x;
    s_btn_sel = lv_btn_create(s_page_cleaning);
    lv_obj_t *btn_sel = s_btn_sel;
    lv_obj_remove_style_all(btn_sel);
    lv_obj_add_style(btn_sel, &g_sty_btn_active, 0);
    lv_obj_set_size(btn_sel, sel_w, rep_h);
    lv_obj_set_pos(btn_sel, sel_x, frame2_y);
    lv_obj_add_event_cb(btn_sel, cb_vac_sel, LV_EVENT_CLICKED, nullptr);
    {
        lv_obj_t *l = lv_label_create(btn_sel);
        lv_obj_add_style(l, &g_sty_lbl_lg, 0);
        lv_label_set_text(l, "Start Selected");
        lv_obj_center(l);
    }

    apply_repeat_style();

    // ── Vacuum controls: All / Pause / Dock ───────────────────────────────────
    const int32_t ctrl_y = frame2_y + rep_h + GAP * 2;
    const int32_t ctrl_h = 90;
    const int32_t ctrl_w = (inner_w - GAP * 2) / 3;

    struct { const char *icon; const char *label; lv_event_cb_t cb; } ctrl[3] = {
        { MDI_PLAY,        "All",   cb_vac_all   },
        { MDI_PAUSE,       "Pause", cb_vac_pause },
        { MDI_HOME_IMPORT, "Dock",  cb_vac_dock  },
    };

    for (int i = 0; i < 3; i++) {
        make_icon_btn(s_page_cleaning,
                      i * (ctrl_w + GAP), ctrl_y,
                      ctrl_w, ctrl_h,
                      ctrl[i].icon, ctrl[i].label,
                      ctrl[i].cb, nullptr);
    }

    // ── Trash row ─────────────────────────────────────────────────────────────
    const int32_t trash_y = ctrl_y + ctrl_h + GAP;
    const int32_t trash_w = ctrl_w * 2 + GAP;
    make_icon_btn(s_page_cleaning,
                  (inner_w - trash_w) / 2, trash_y,
                  trash_w, ctrl_h,
                  MDI_TRASH_CAN, "Trash",
                  cb_vac_trash, nullptr);
}

lv_obj_t *get_page_cleaning() { return s_page_cleaning; }

// ── Update functions ──────────────────────────────────────────────────────────
bool cleaning_page_update_switch(const char *topic, bool on) {
    for (int i = 0; i < 6; i++) {
        if (strcmp(topic, s_vac_rooms[i].topic) == 0) {
            s_vac_rooms[i].on = on;
            apply_vac_room_style(&s_vac_rooms[i]);
            return true;
        }
    }
    return false;
}

bool cleaning_page_update_sensor(const char *topic, const char *value) {
    if (strcmp(topic, TOPIC_VAC_REPEAT) == 0) {
        strlcpy(s_vac_repeat, value, sizeof(s_vac_repeat));
        apply_repeat_style();
        return true;
    }
    return false;
}
