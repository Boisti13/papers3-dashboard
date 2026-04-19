#include "bedroom_page.h"
#include "../../styles.h"
#include "../../nav.h"
#include "../../../config.h"
#include "../../../mqtt/ha_mqtt.h"

#include <string.h>

// ── Light toggle struct ───────────────────────────────────────────────────────
struct BedLight {
    const char *name;
    const char *topic_state;
    const char *topic_cmd;
    lv_obj_t   *btn;
    bool        on;
};

static BedLight s_bed_lights[2] = {
    { "Nightstand", TOPIC_BED_LIGHT_STATE,      TOPIC_BED_LIGHT_CMD,      nullptr, false },
    { "Bed light",  TOPIC_BED_NIGHTSTAND_STATE, TOPIC_BED_NIGHTSTAND_CMD, nullptr, false },
};

static lv_obj_t *s_page_bedroom = nullptr;

// ── Style helper ──────────────────────────────────────────────────────────────
static void apply_bed_light_style(BedLight *bl) {
    if (!bl->btn) return;
    lv_obj_set_style_border_width(bl->btn, bl->on ? 4 : 2, 0);
    lv_obj_set_style_bg_color(bl->btn,
        bl->on ? lv_color_make(0xDD, 0xDD, 0xDD) : lv_color_white(), 0);
}

// ── Callback ──────────────────────────────────────────────────────────────────
static void cb_bed_light(lv_event_t *e) {
    BedLight *bl = (BedLight *)lv_event_get_user_data(e);
    bl->on = !bl->on;
    apply_bed_light_style(bl);
    mqtt_publish(bl->topic_cmd, bl->on ? "ON" : "OFF");
}

// ── Page builder ──────────────────────────────────────────────────────────────
void build_page_bedroom() {
    s_page_bedroom = make_page();

    const int32_t inner_w = EPD_WIDTH - PAD * 2;   // 508
    const int32_t btn_h   = 100;                    // same as cleaning room tiles
    const int32_t btn_w   = (inner_w - GAP) / 2;   // two equal columns

    for (int i = 0; i < 2; i++) {
        int32_t x = i * (btn_w + GAP);

        lv_obj_t *btn = lv_btn_create(s_page_bedroom);
        lv_obj_remove_style_all(btn);
        lv_obj_add_style(btn, &g_sty_btn, 0);
        lv_obj_set_size(btn, btn_w, btn_h);
        lv_obj_set_pos(btn, x, 0);
        lv_obj_set_style_radius(btn, 10, 0);
        lv_obj_add_event_cb(btn, cb_bed_light, LV_EVENT_CLICKED, &s_bed_lights[i]);
        s_bed_lights[i].btn = btn;

        lv_obj_t *lbl = lv_label_create(btn);
        lv_obj_add_style(lbl, &g_sty_lbl_lg, 0);
        lv_label_set_text(lbl, s_bed_lights[i].name);
        lv_obj_center(lbl);

        apply_bed_light_style(&s_bed_lights[i]);
    }
}

lv_obj_t *get_page_bedroom() { return s_page_bedroom; }

// ── Update functions ──────────────────────────────────────────────────────────
bool bedroom_page_update_switch(const char *topic, bool on) {
    for (int i = 0; i < 2; i++) {
        if (strcmp(topic, s_bed_lights[i].topic_state) == 0) {
            s_bed_lights[i].on = on;
            apply_bed_light_style(&s_bed_lights[i]);
            return true;
        }
    }
    return false;
}

bool bedroom_page_update_sensor(const char *topic, const char *value) {
    (void)topic; (void)value;
    return false;
}
