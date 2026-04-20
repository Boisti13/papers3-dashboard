#include "switches_page.h"
#include "../../styles.h"
#include "../../nav.h"
#include "../../../config.h"
#include "../../../mqtt/ha_mqtt.h"
#include "../../../fonts/icons.h"

#include <string.h>

// ── Layout ────────────────────────────────────────────────────────────────────
#define SW_COLS   2
#define SW_ROWS   4
#define SW_TILE_W ((EPD_WIDTH  - PAD * 2 - GAP * (SW_COLS - 1)) / SW_COLS)
#define SW_TILE_H ((CONTENT_H  - PAD * 2 - GAP * (SW_ROWS - 1)) / SW_ROWS)

// ── SwTile struct ─────────────────────────────────────────────────────────────
struct SwTile {
    const char *name;
    const char *icon;
    const char *topic_state;
    const char *topic_cmd;
    lv_obj_t   *btn;
    lv_obj_t   *lbl_status;
    bool        on;
};

static SwTile s_sw_tiles[8] = {
    { "TV",          MDI_TELEVISION,      TOPIC_SW_TV_STATE,       TOPIC_SW_TV_CMD,       nullptr, nullptr, false },
    { "TV Socket",   MDI_POWER_SOCKET,    TOPIC_SW_TVSOCK_STATE,   TOPIC_SW_TVSOCK_CMD,   nullptr, nullptr, false },
    { "Shield",      MDI_NVIDIA,          TOPIC_SW_SHIELD_STATE,   TOPIC_SW_SHIELD_CMD,   nullptr, nullptr, false },
    { "Living LED",  MDI_LIGHTBULB,       TOPIC_SW_LIVING_STATE,   TOPIC_SW_LIVING_CMD,   nullptr, nullptr, false },
    { "Kitchen",     MDI_SILVERWARE_FORK, TOPIC_SW_KITCHEN_STATE,  TOPIC_SW_KITCHEN_CMD,  nullptr, nullptr, false },
    { "Heatlamp",    MDI_HEAT_WAVE,       TOPIC_SW_HEATLAMP_STATE, TOPIC_SW_HEATLAMP_CMD, nullptr, nullptr, false },
    { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, false },
    { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, false },
};

static lv_obj_t *s_page_switches = nullptr;

// ── Style helpers ─────────────────────────────────────────────────────────────
static void apply_sw_tile_style(SwTile *t) {
    if (t->on) {
        lv_obj_remove_style(t->btn, &g_sty_btn,        LV_PART_MAIN);
        lv_obj_add_style   (t->btn, &g_sty_btn_active,  LV_PART_MAIN);
    } else {
        lv_obj_remove_style(t->btn, &g_sty_btn_active,  LV_PART_MAIN);
        lv_obj_add_style   (t->btn, &g_sty_btn,         LV_PART_MAIN);
    }
    lv_label_set_text(t->lbl_status, t->on ? "ON" : "OFF");
    lv_obj_invalidate(t->btn);
}

static void cb_sw_tile(lv_event_t *e) {
    SwTile *t = (SwTile *)lv_event_get_user_data(e);
    t->on = !t->on;
    apply_sw_tile_style(t);
    mqtt_publish(t->topic_cmd, t->on ? "ON" : "OFF");
}

// ── Page builder ──────────────────────────────────────────────────────────────
void build_page_switches() {
    s_page_switches = make_page();

    for (int i = 0; i < SW_COLS * SW_ROWS; i++) {
        int     row  = i / SW_COLS;
        int     col  = i % SW_COLS;
        int32_t x    = col * (SW_TILE_W + GAP);
        int32_t y    = row * (SW_TILE_H + GAP);
        bool    empty = (s_sw_tiles[i].name == nullptr);

        lv_obj_t *btn = lv_btn_create(s_page_switches);
        lv_obj_remove_style_all(btn);
        lv_obj_add_style(btn, &g_sty_btn, LV_PART_MAIN);
        lv_obj_set_size(btn, SW_TILE_W, SW_TILE_H);
        lv_obj_set_pos(btn, x, y);
        lv_obj_set_style_radius(btn, 12, 0);
        s_sw_tiles[i].btn = btn;

        if (empty) continue;

        lv_obj_add_event_cb(btn, cb_sw_tile, LV_EVENT_CLICKED, &s_sw_tiles[i]);

        // Status label — top-right
        s_sw_tiles[i].lbl_status = lv_label_create(btn);
        lv_obj_add_style(s_sw_tiles[i].lbl_status, &g_sty_lbl_lg, 0);
        lv_label_set_text(s_sw_tiles[i].lbl_status, "---");
        lv_obj_align(s_sw_tiles[i].lbl_status, LV_ALIGN_TOP_RIGHT, -8, 8);

        // MDI icon — centered (slightly above centre)
        lv_obj_t *ico = lv_label_create(btn);
        lv_obj_set_style_text_font(ico, &lv_font_mdi_48, 0);
        lv_obj_set_style_text_color(ico, lv_color_black(), 0);
        lv_label_set_text(ico, s_sw_tiles[i].icon);
        lv_obj_align(ico, LV_ALIGN_CENTER, 0, -14);

        // Name label — bottom-centre
        lv_obj_t *lbl_name = lv_label_create(btn);
        lv_obj_add_style(lbl_name, &g_sty_lbl_lg, 0);
        lv_label_set_text(lbl_name, s_sw_tiles[i].name);
        lv_obj_align(lbl_name, LV_ALIGN_BOTTOM_MID, 0, -8);

        apply_sw_tile_style(&s_sw_tiles[i]);
    }
}

lv_obj_t *get_page_switches() { return s_page_switches; }

// ── Update functions ──────────────────────────────────────────────────────────
bool switches_page_update_switch(const char *topic, bool on) {
    for (int i = 0; i < SW_COLS * SW_ROWS; i++) {
        if (!s_sw_tiles[i].topic_state) continue;
        if (strcmp(topic, s_sw_tiles[i].topic_state) == 0) {
            s_sw_tiles[i].on = on;
            if (s_sw_tiles[i].btn) apply_sw_tile_style(&s_sw_tiles[i]);
            return true;
        }
    }
    return false;
}

bool switches_page_update_sensor(const char *topic, const char *value) {
    (void)topic; (void)value;
    return false;
}
