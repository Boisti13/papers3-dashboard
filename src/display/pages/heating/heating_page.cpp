#include "heating_page.h"
#include "../../styles.h"
#include "../../nav.h"
#include "../../../config.h"
#include "../../../mqtt/ha_mqtt.h"
#include "../../../fonts/icons.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// ── Layout ────────────────────────────────────────────────────────────────────
#define HEAT_TILE_W  ((EPD_WIDTH - PAD * 2 - GAP) / 2)
#define HEAT_TILE_H  130
#define ICON_GAP     58   // horizontal gap: icon left edge → label left edge

// ── RoomHeat struct ───────────────────────────────────────────────────────────
struct RoomHeat {
    const char *name;
    const char *entity_id;     // e.g. "bathroom_radiator" — used for paperdash/ command topics
    const char *topic_temp;
    const char *topic_hum;
    const char *topic_target;
    const char *topic_action;
    const char *topic_preset;
    const char *topic_window;
    const char *topic_set;
    const char *topic_mode;    // hvac_mode state (heat / off)
    lv_obj_t   *page;
    lv_obj_t   *lbl_setpoint;
    lv_obj_t   *lbl_temp;
    lv_obj_t   *lbl_hum;
    lv_obj_t   *lbl_action;
    lv_obj_t   *lbl_action_ico;
    lv_obj_t   *lbl_window;
    lv_obj_t   *btn_frost;
    lv_obj_t   *btn_eco;
    lv_obj_t   *btn_comfort;
    lv_obj_t   *btn_boost;
    lv_obj_t   *btn_onoff;
    lv_obj_t   *lbl_onoff;
    float       target_val;
    char        preset[16];
    bool        hvac_on;
};

static RoomHeat g_rooms[5] = {
    { "Bathroom",    "bathroom_radiator",
      TOPIC_HEAT_BATH_TEMP,   TOPIC_HEAT_BATH_HUM,    TOPIC_HEAT_BATH_TARGET,
      TOPIC_HEAT_BATH_ACTION, TOPIC_HEAT_BATH_PRESET,  TOPIC_HEAT_BATH_WINDOW,
      TOPIC_HEAT_BATH_SET,    TOPIC_HEAT_BATH_MODE,
      nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
      20.0f, "", false },
    { "Kitchen",     "kitchen_radiator",
      TOPIC_HEAT_KTCH_TEMP,   TOPIC_HEAT_KTCH_HUM,    TOPIC_HEAT_KTCH_TARGET,
      TOPIC_HEAT_KTCH_ACTION, TOPIC_HEAT_KTCH_PRESET,  TOPIC_HEAT_KTCH_WINDOW,
      TOPIC_HEAT_KTCH_SET,    TOPIC_HEAT_KTCH_MODE,
      nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
      20.0f, "", false },
    { "Bedroom",     "bedroom_radiator",
      TOPIC_HEAT_BED_TEMP,    TOPIC_HEAT_BED_HUM,     TOPIC_HEAT_BED_TARGET,
      TOPIC_HEAT_BED_ACTION,  TOPIC_HEAT_BED_PRESET,   TOPIC_HEAT_BED_WINDOW,
      TOPIC_HEAT_BED_SET,     TOPIC_HEAT_BED_MODE,
      nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
      20.0f, "", false },
    { "Living Room", "living_room_radiator",
      TOPIC_HEAT_LIVE_TEMP,   TOPIC_HEAT_LIVE_HUM,    TOPIC_HEAT_LIVE_TARGET,
      TOPIC_HEAT_LIVE_ACTION, TOPIC_HEAT_LIVE_PRESET,  TOPIC_HEAT_LIVE_WINDOW,
      TOPIC_HEAT_LIVE_SET,    TOPIC_HEAT_LIVE_MODE,
      nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
      20.0f, "", false },
    { "Arthur",      "arthur_radiator",
      TOPIC_HEAT_ART_TEMP,    TOPIC_HEAT_ART_HUM,     TOPIC_HEAT_ART_TARGET,
      TOPIC_HEAT_ART_ACTION,  TOPIC_HEAT_ART_PRESET,   TOPIC_HEAT_ART_WINDOW,
      TOPIC_HEAT_ART_SET,     TOPIC_HEAT_ART_MODE,
      nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
      20.0f, "", false },
};

static lv_obj_t *s_page_heating = nullptr;

// ── Helpers ───────────────────────────────────────────────────────────────────
static bool is_on(const char *v) {
    return strcasecmp(v, "on") == 0 || strcmp(v, "1") == 0 || strcasecmp(v, "true") == 0;
}

static lv_obj_t *make_mdi_label(lv_obj_t *parent, int32_t x, int32_t y, const char *icon) {
    lv_obj_t *lbl = lv_label_create(parent);
    lv_obj_set_style_text_font(lbl, &lv_font_mdi_48, 0);
    lv_obj_set_style_text_color(lbl, lv_color_black(), 0);
    lv_label_set_text(lbl, icon);
    lv_obj_set_pos(lbl, x, y);
    return lbl;
}

// ── ON/OFF button helpers ─────────────────────────────────────────────────────
static void apply_onoff_style(RoomHeat *r) {
    if (!r->btn_onoff || !r->lbl_onoff) return;
    lv_label_set_text(r->lbl_onoff, r->hvac_on ? "Heating: ON" : "Heating: OFF");
    lv_obj_set_style_bg_color(r->btn_onoff,
        r->hvac_on ? lv_color_make(0xF2, 0xF2, 0xF2) : lv_color_white(), 0);
    lv_obj_set_style_border_width(r->btn_onoff, r->hvac_on ? 4 : 2, 0);
}

static void cb_onoff(lv_event_t *e) {
    RoomHeat *r = (RoomHeat *)lv_event_get_user_data(e);
    r->hvac_on = !r->hvac_on;
    apply_onoff_style(r);
    char topic[128];
    snprintf(topic, sizeof(topic), "paperdash/climate/%s/set_hvac_mode", r->entity_id);
    mqtt_publish(topic, r->hvac_on ? "heat" : "off");
}

// ── Preset buttons ────────────────────────────────────────────────────────────
struct PresetParam { RoomHeat *r; const char *preset; };
static PresetParam s_preset_params[5][4];

static void update_preset_indicators(RoomHeat *r) {
    const char *names[] = { "frost", "eco", "comfort", "boost" };
    lv_obj_t   *btns[]  = { r->btn_frost, r->btn_eco, r->btn_comfort, r->btn_boost };
    for (int i = 0; i < 4; i++) {
        if (!btns[i]) continue;
        bool active = strcasecmp(r->preset, names[i]) == 0;
        lv_obj_set_style_border_width(btns[i], active ? 4 : 2, 0);
        lv_obj_set_style_bg_color(btns[i],
            active ? lv_color_make(0xF2, 0xF2, 0xF2) : lv_color_white(), 0);
    }
}

static void cb_preset(lv_event_t *e) {
    PresetParam *p = (PresetParam *)lv_event_get_user_data(e);
    char topic[128];
    snprintf(topic, sizeof(topic), "paperdash/climate/%s/set_preset_mode", p->r->entity_id);
    mqtt_publish(topic, p->preset);
}

static void cb_room_sp_dec(lv_event_t *e) {
    RoomHeat *r = (RoomHeat *)lv_event_get_user_data(e);
    r->target_val -= 0.5f;
    if (r->target_val < 5.0f) r->target_val = 5.0f;
    char buf[12];
    snprintf(buf, sizeof(buf), "%.1f \xc2\xb0""C", r->target_val);
    lv_label_set_text(r->lbl_setpoint, buf);
    char topic[128];
    snprintf(topic, sizeof(topic), "paperdash/climate/%s/set_temperature", r->entity_id);
    mqtt_publish_float(topic, r->target_val);
}

static void cb_room_sp_inc(lv_event_t *e) {
    RoomHeat *r = (RoomHeat *)lv_event_get_user_data(e);
    r->target_val += 0.5f;
    if (r->target_val > 30.0f) r->target_val = 30.0f;
    char buf[12];
    snprintf(buf, sizeof(buf), "%.1f \xc2\xb0""C", r->target_val);
    lv_label_set_text(r->lbl_setpoint, buf);
    char topic[128];
    snprintf(topic, sizeof(topic), "paperdash/climate/%s/set_temperature", r->entity_id);
    mqtt_publish_float(topic, r->target_val);
    // If heating is off, switch to heat mode automatically
    if (!r->hvac_on) {
        r->hvac_on = true;
        apply_onoff_style(r);
        snprintf(topic, sizeof(topic), "paperdash/climate/%s/set_hvac_mode", r->entity_id);
        mqtt_publish(topic, "heat");
    }
}

static lv_obj_t *make_preset_btn(lv_obj_t *parent, int32_t x, int32_t y,
                                  const char *icon, PresetParam *param) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_remove_style_all(btn);
    lv_obj_add_style(btn, &g_sty_btn, 0);
    lv_obj_set_size(btn, 80, 80);
    lv_obj_set_pos(btn, x, y);
    lv_obj_add_event_cb(btn, cb_preset, LV_EVENT_CLICKED, param);
    lv_obj_t *lbl = lv_label_create(btn);
    lv_obj_set_style_text_font(lbl, &lv_font_mdi_48, 0);
    lv_obj_set_style_text_color(lbl, lv_color_black(), 0);
    lv_label_set_text(lbl, icon);
    lv_obj_center(lbl);
    return btn;
}

// ── Room detail page ──────────────────────────────────────────────────────────
static void build_room_page(int idx) {
    RoomHeat *r = &g_rooms[idx];
    r->page = make_page();

    const int32_t inner_w = EPD_WIDTH - PAD * 2;  // 508
    const int32_t col2_x  = inner_w / 2 + 4;      // 258

    // ── Setpoint row (y=0) ───────────────────────────────────────────────────
    const int32_t sp_bw = 120, sp_bh = 120;

    lv_obj_t *btn_dec = lv_btn_create(r->page);
    lv_obj_remove_style_all(btn_dec);
    lv_obj_add_style(btn_dec, &g_sty_btn, 0);
    lv_obj_set_size(btn_dec, sp_bw, sp_bh);
    lv_obj_set_pos(btn_dec, 0, 0);
    lv_obj_add_event_cb(btn_dec, cb_room_sp_dec, LV_EVENT_CLICKED, r);
    lv_obj_t *lbl_dec = lv_label_create(btn_dec);
    lv_obj_add_style(lbl_dec, &g_sty_lbl_lg, 0);
    lv_label_set_text(lbl_dec, "-");
    lv_obj_center(lbl_dec);

    r->lbl_setpoint = lv_label_create(r->page);
    lv_obj_add_style(r->lbl_setpoint, &g_sty_lbl_lg, 0);
    lv_label_set_text(r->lbl_setpoint, "--.- \xc2\xb0""C");
    lv_obj_set_width(r->lbl_setpoint, inner_w - sp_bw * 2);
    lv_obj_set_style_text_align(r->lbl_setpoint, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_pos(r->lbl_setpoint, sp_bw, (sp_bh - 34) / 2);

    lv_obj_t *btn_inc = lv_btn_create(r->page);
    lv_obj_remove_style_all(btn_inc);
    lv_obj_add_style(btn_inc, &g_sty_btn, 0);
    lv_obj_set_size(btn_inc, sp_bw, sp_bh);
    lv_obj_set_pos(btn_inc, inner_w - sp_bw, 0);
    lv_obj_add_event_cb(btn_inc, cb_room_sp_inc, LV_EVENT_CLICKED, r);
    lv_obj_t *lbl_inc = lv_label_create(btn_inc);
    lv_obj_add_style(lbl_inc, &g_sty_lbl_lg, 0);
    lv_label_set_text(lbl_inc, "+");
    lv_obj_center(lbl_inc);

    // ── HVAC + Window row (y=140) ────────────────────────────────────────────
    const int32_t row1_y = 140;

    r->lbl_action_ico = make_mdi_label(r->page, 0, row1_y, MDI_RADIATOR_DISABLED);
    r->lbl_action     = make_info_label(r->page, ICON_GAP, row1_y + 14, "Off");

    make_mdi_label(r->page, col2_x, row1_y, MDI_WINDOW_OPEN);
    r->lbl_window = make_info_label(r->page, col2_x + ICON_GAP, row1_y + 14, "--");

    // ── Temp + Hum row (y=210) ───────────────────────────────────────────────
    const int32_t row2_y = 210;

    make_mdi_label(r->page, 0, row2_y, MDI_THERMOMETER);
    r->lbl_temp = make_value_label(r->page, ICON_GAP, row2_y + 6, "--.- \xc2\xb0""C");

    make_mdi_label(r->page, col2_x, row2_y, MDI_WATER_PERCENT);
    r->lbl_hum  = make_value_label(r->page, col2_x + ICON_GAP, row2_y + 6, "-- %");

    // ── Preset icon buttons (y=300) ──────────────────────────────────────────
    const int32_t preset_y   = 300;
    const int32_t pbtn_w     = 80;
    const int32_t pbtn_gap   = (inner_w - 4 * pbtn_w) / 3;  // ≈ 62

    static const char *preset_icons[] = { MDI_SNOWFLAKE, MDI_LEAF, MDI_SOFA, MDI_ROCKET };
    static const char *preset_keys[]  = { "frost", "eco", "comfort", "boost" };

    for (int pi = 0; pi < 4; pi++) {
        s_preset_params[idx][pi] = { r, preset_keys[pi] };
        int32_t px = pi * (pbtn_w + pbtn_gap);
        lv_obj_t *pb = make_preset_btn(r->page, px, preset_y,
                                        preset_icons[pi], &s_preset_params[idx][pi]);
        if      (pi == 0) r->btn_frost   = pb;
        else if (pi == 1) r->btn_eco     = pb;
        else if (pi == 2) r->btn_comfort = pb;
        else              r->btn_boost   = pb;
    }

    // ── ON/OFF toggle button (y=400) ─────────────────────────────────────────
    const int32_t onoff_y = 400;

    r->btn_onoff = lv_btn_create(r->page);
    lv_obj_remove_style_all(r->btn_onoff);
    lv_obj_add_style(r->btn_onoff, &g_sty_btn, 0);
    lv_obj_set_size(r->btn_onoff, inner_w, 80);
    lv_obj_set_pos(r->btn_onoff, 0, onoff_y);
    lv_obj_add_event_cb(r->btn_onoff, cb_onoff, LV_EVENT_CLICKED, r);

    r->lbl_onoff = lv_label_create(r->btn_onoff);
    lv_obj_set_style_text_font(r->lbl_onoff, &lv_font_montserrat_36, 0);
    lv_obj_set_style_text_color(r->lbl_onoff, lv_color_black(), 0);
    lv_label_set_text(r->lbl_onoff, "Heating: OFF");
    lv_obj_center(r->lbl_onoff);

    apply_onoff_style(r);
}

// ── Heating list page ─────────────────────────────────────────────────────────
static void cb_room_btn(lv_event_t *e) {
    RoomHeat *r = (RoomHeat *)lv_event_get_user_data(e);
    navigate_to(r->page, r->name);
}

void build_page_heating() {
    s_page_heating = make_page();

    const int32_t inner_w = EPD_WIDTH - PAD * 2;
    const int32_t tile_w  = (inner_w - GAP) / 2;
    const int32_t tile_h  = HEAT_TILE_H;

    static const char *room_icons[5] = {
        MDI_SHOWER, MDI_SILVERWARE_FORK, MDI_BED, MDI_SOFA_OUTLINE, MDI_HUMAN_MALE_BOY
    };

    for (int i = 0; i < 5; i++) {
        int     row = i / 2;
        int     col = i % 2;
        int32_t x   = col * (tile_w + GAP);
        int32_t y   = row * (tile_h + GAP);
        if (i == 4) x = (inner_w - tile_w) / 2;

        lv_obj_t *btn = lv_btn_create(s_page_heating);
        lv_obj_remove_style_all(btn);
        lv_obj_add_style(btn, &g_sty_btn_grid, 0);
        lv_obj_set_size(btn, tile_w, tile_h);
        lv_obj_set_pos(btn, x, y);
        lv_obj_add_event_cb(btn, cb_room_btn, LV_EVENT_CLICKED, &g_rooms[i]);

        // Icon (upper area)
        lv_obj_t *ico = lv_label_create(btn);
        lv_obj_set_style_text_font(ico, &lv_font_mdi_48, 0);
        lv_obj_set_style_text_color(ico, lv_color_black(), 0);
        lv_label_set_text(ico, room_icons[i]);
        lv_obj_align(ico, LV_ALIGN_CENTER, 0, -14);

        // Name (lower area)
        lv_obj_t *lbl = lv_label_create(btn);
        lv_obj_add_style(lbl, &g_sty_lbl_lg, 0);
        lv_label_set_text(lbl, g_rooms[i].name);
        lv_obj_align(lbl, LV_ALIGN_BOTTOM_MID, 0, -10);
    }

    for (int i = 0; i < 5; i++) build_room_page(i);
}

lv_obj_t *get_page_heating() { return s_page_heating; }

// ── Sensor update ─────────────────────────────────────────────────────────────
bool heating_page_update_sensor(const char *topic, const char *value) {
    for (int i = 0; i < 5; i++) {
        RoomHeat *r = &g_rooms[i];

        if (strcmp(topic, r->topic_temp) == 0) {
            char buf[16];
            snprintf(buf, sizeof(buf), "%s \xc2\xb0""C", value);
            if (r->lbl_temp) lv_label_set_text(r->lbl_temp, buf);
            return true;
        }
        if (strcmp(topic, r->topic_hum) == 0) {
            char buf[16];
            snprintf(buf, sizeof(buf), "%s %%", value);
            if (r->lbl_hum) lv_label_set_text(r->lbl_hum, buf);
            return true;
        }
        if (strcmp(topic, r->topic_target) == 0) {
            r->target_val = atof(value);
            char buf[12];
            snprintf(buf, sizeof(buf), "%.1f \xc2\xb0""C", r->target_val);
            if (r->lbl_setpoint) lv_label_set_text(r->lbl_setpoint, buf);
            return true;
        }
        if (strcmp(topic, r->topic_action) == 0) {
            // Strip statestream quotes
            const char *v = value;
            if (v[0] == '"') v++;
            char stripped[32];
            strlcpy(stripped, v, sizeof(stripped));
            size_t sl = strlen(stripped);
            if (sl > 0 && stripped[sl - 1] == '"') stripped[sl - 1] = '\0';
            if (r->lbl_action) lv_label_set_text(r->lbl_action, stripped);
            if (r->lbl_action_ico) {
                bool heating = strcasecmp(stripped, "heating") == 0;
                lv_label_set_text(r->lbl_action_ico,
                                  heating ? MDI_FIRE : MDI_RADIATOR_DISABLED);
            }
            return true;
        }
        if (strcmp(topic, r->topic_preset) == 0) {
            // Statestream wraps strings in quotes — strip them
            const char *v = value;
            if (v[0] == '"') v++;
            strlcpy(r->preset, v, sizeof(r->preset));
            size_t len = strlen(r->preset);
            if (len > 0 && r->preset[len - 1] == '"') r->preset[len - 1] = '\0';
            update_preset_indicators(r);
            return true;
        }
        if (strcmp(topic, r->topic_window) == 0) {
            if (r->lbl_window)
                lv_label_set_text(r->lbl_window, is_on(value) ? "Open" : "Closed");
            return true;
        }
        if (strcmp(topic, r->topic_mode) == 0) {
            // Strip statestream quotes
            const char *v = value;
            if (v[0] == '"') v++;
            r->hvac_on = strncasecmp(v, "heat", 4) == 0;
            apply_onoff_style(r);
            return true;
        }
    }
    return false;
}

bool heating_page_update_switch(const char *topic, bool on) {
    (void)topic; (void)on;
    return false;
}
