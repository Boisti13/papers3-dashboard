#include "climate_page.h"
#include "../../styles.h"
#include "../../nav.h"
#include "../../../config.h"

#include <string.h>
#include <stdio.h>

static lv_obj_t *s_page_climate = nullptr;
static lv_obj_t *s_lbl_out_temp = nullptr;
static lv_obj_t *s_lbl_out_hum  = nullptr;
static lv_obj_t *s_lbl_co2      = nullptr;
static lv_obj_t *s_lbl_pm25     = nullptr;

// ── Helper: one content row (label left, value right, same font/y) ────────────
static lv_obj_t *make_row(lv_obj_t *parent, int32_t y,
                           const char *label_text, const char *value_text) {
    lv_obj_t *lbl = lv_label_create(parent);
    lv_obj_add_style(lbl, &g_sty_lbl_md, 0);
    lv_label_set_text(lbl, label_text);
    lv_obj_set_pos(lbl, 0, y);

    lv_obj_t *val = lv_label_create(parent);
    lv_obj_add_style(val, &g_sty_lbl_md, 0);
    lv_label_set_text(val, value_text);
    lv_obj_set_pos(val, 220, y);

    return val;
}

void build_page_climate() {
    s_page_climate = make_page();

    const int32_t card_w  = EPD_WIDTH - PAD * 2;  // 508
    const int32_t card_h  = 140;                   // outer height
    const int32_t hdr_y   = 0;                     // headline y (28pt)
    const int32_t row1_y  = 44;                    // first content row y (20pt)
    const int32_t row2_y  = 76;                    // second content row y (20pt)

    // ── Card 1: Outdoor ───────────────────────────────────────────────────────
    lv_obj_t *card1 = lv_obj_create(s_page_climate);
    lv_obj_remove_style_all(card1);
    lv_obj_add_style(card1, &g_sty_card, 0);
    lv_obj_set_size(card1, card_w, card_h);
    lv_obj_set_pos(card1, 0, 0);
    lv_obj_clear_flag(card1, LV_OBJ_FLAG_SCROLLABLE);

    {
        lv_obj_t *h = lv_label_create(card1);
        lv_obj_add_style(h, &g_sty_lbl_lg, 0);
        lv_label_set_text(h, "Outdoor");
        lv_obj_set_pos(h, 0, hdr_y);

        s_lbl_out_temp = make_row(card1, row1_y, "Temperature:", "--.- \xc2\xb0""C");
        s_lbl_out_hum  = make_row(card1, row2_y, "Humidity:",    "-- %");
    }

    // ── Card 2: Air Quality ───────────────────────────────────────────────────
    lv_obj_t *card2 = lv_obj_create(s_page_climate);
    lv_obj_remove_style_all(card2);
    lv_obj_add_style(card2, &g_sty_card, 0);
    lv_obj_set_size(card2, card_w, card_h);
    lv_obj_set_pos(card2, 0, card_h + GAP);
    lv_obj_clear_flag(card2, LV_OBJ_FLAG_SCROLLABLE);

    {
        lv_obj_t *h = lv_label_create(card2);
        lv_obj_add_style(h, &g_sty_lbl_lg, 0);
        lv_label_set_text(h, "Air Quality");
        lv_obj_set_pos(h, 0, hdr_y);

        s_lbl_co2  = make_row(card2, row1_y, "CO2:",   "-- ppm");
        s_lbl_pm25 = make_row(card2, row2_y, "PM2.5:", "-- ug/m3");
    }
}

lv_obj_t *get_page_climate() { return s_page_climate; }

bool climate_page_update_sensor(const char *topic, const char *value) {
    char buf[32];

    if (strcmp(topic, TOPIC_OUT_TEMP) == 0 && s_lbl_out_temp) {
        snprintf(buf, sizeof(buf), "%s \xc2\xb0""C", value);
        lv_label_set_text(s_lbl_out_temp, buf);
        return true;
    }
    if (strcmp(topic, TOPIC_OUT_HUM) == 0 && s_lbl_out_hum) {
        snprintf(buf, sizeof(buf), "%s %%", value);
        lv_label_set_text(s_lbl_out_hum, buf);
        return true;
    }
    if (strcmp(topic, TOPIC_CO2) == 0 && s_lbl_co2) {
        snprintf(buf, sizeof(buf), "%s ppm", value);
        lv_label_set_text(s_lbl_co2, buf);
        return true;
    }
    if (strcmp(topic, TOPIC_PM25) == 0 && s_lbl_pm25) {
        snprintf(buf, sizeof(buf), "%s ug/m3", value);
        lv_label_set_text(s_lbl_pm25, buf);
        return true;
    }
    return false;
}

bool climate_page_update_switch(const char *topic, bool on) {
    (void)topic; (void)on;
    return false;
}
