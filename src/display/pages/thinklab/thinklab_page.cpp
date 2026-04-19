#include "thinklab_page.h"
#include "../../styles.h"
#include "../../nav.h"
#include "../../../config.h"
#include "../../../mqtt/ha_mqtt.h"

#include <string.h>
#include <stdio.h>

// ── TlService struct ──────────────────────────────────────────────────────────
struct TlService {
    const char *name;
    const char *topic_state;
    const char *topic_cmd;
    lv_obj_t   *btn;
    lv_obj_t   *lbl_status;
    bool        on;
};

static TlService s_tl_svc[7] = {
    { "Audiobookshelf", TOPIC_TL_LXC_ABS,  TOPIC_TL_LXC_ABS_CMD,  nullptr, nullptr, false },
    { "Jellyfin",       TOPIC_TL_LXC_JEL,  TOPIC_TL_LXC_JEL_CMD,  nullptr, nullptr, false },
    { "Pi-hole",        TOPIC_TL_LXC_PIH,  TOPIC_TL_LXC_PIH_CMD,  nullptr, nullptr, false },
    { "SMB Share",      TOPIC_TL_LXC_SMB,  TOPIC_TL_LXC_SMB_CMD,  nullptr, nullptr, false },
    { "ZeroTier",       TOPIC_TL_LXC_ZT,   TOPIC_TL_LXC_ZT_CMD,   nullptr, nullptr, false },
    { "Sprout Track",   TOPIC_TL_LXC_SPR,  TOPIC_TL_LXC_SPR_CMD,  nullptr, nullptr, false },
    { "HAOS VM",        TOPIC_TL_VM_HAOS,  TOPIC_TL_VM_HAOS_CMD,  nullptr, nullptr, false },
};

static lv_obj_t *s_page_thinklab  = nullptr;
static lv_obj_t *s_lbl_tl_cpu     = nullptr;
static lv_obj_t *s_lbl_tl_disk[5] = {};

// Disk layout: 2 columns, 3 rows (last cell empty)
// Index mapping: [0]=SSD256, [1]=HDD500, [2]=HDD5TB, [3]=HDD2TB, [4]=HDD8TB
static const char *k_disk_labels[5] = {
    "SSD 256G", "HDD 500G", "HDD 5TB", "HDD 2TB", "HDD 8TB"
};

// ── Style helpers ─────────────────────────────────────────────────────────────
static void apply_svc_style(TlService *s) {
    if (!s->btn || !s->lbl_status) return;
    lv_label_set_text(s->lbl_status, s->on ? "ON" : "OFF");
    lv_obj_set_style_bg_color(s->btn,
        s->on ? lv_color_make(0xF4, 0xF4, 0xF4) : lv_color_white(), 0);
    lv_obj_set_style_border_width(s->btn, s->on ? 4 : 2, 0);
}

// ── Callbacks ─────────────────────────────────────────────────────────────────
static void cb_tl_svc(lv_event_t *e) {
    TlService *s = (TlService *)lv_event_get_user_data(e);
    s->on = !s->on;
    apply_svc_style(s);
    mqtt_publish(s->topic_cmd, s->on ? "ON" : "OFF");
}

// ── Disk cell helper ──────────────────────────────────────────────────────────
// Single row: "Label  -- %"  (name left-aligned, value offset by name_w)
static lv_obj_t *make_disk_cell(lv_obj_t *parent, int32_t x, int32_t y,
                                 int32_t cell_w, const char *label) {
    lv_obj_t *lbl_name = lv_label_create(parent);
    lv_obj_add_style(lbl_name, &g_sty_lbl_md, 0);
    lv_label_set_text(lbl_name, label);
    lv_obj_set_pos(lbl_name, x, y);

    // Value label sits to the right of the name; fixed offset keeps columns tidy
    const int32_t val_offset = 150;
    lv_obj_t *lbl_val = lv_label_create(parent);
    lv_obj_add_style(lbl_val, &g_sty_lbl_md, 0);
    lv_label_set_text(lbl_val, "-- %");
    lv_obj_set_pos(lbl_val, x + val_offset, y);

    (void)cell_w;
    return lbl_val;
}

// ── Page builder ──────────────────────────────────────────────────────────────
void build_page_thinklab() {
    s_page_thinklab = make_page();

    const int32_t inner_w  = EPD_WIDTH - PAD * 2;   // 508
    const int32_t sec_hdr_h = 48;                    // section title row height

    // ── Resources section ─────────────────────────────────────────────────────
    // CPU: single full-width row
    // Disks: 2-column grid, 3 rows (5 disks, last cell empty)
    // Each disk cell: name label (20pt) + value label (28pt), stacked
    const int32_t cell_h    = 40;   // single row of 28pt text + gap
    const int32_t cpu_row_h = 44;
    const int32_t disk_cols = 2;
    const int32_t disk_rows = 3;
    const int32_t cell_w    = (inner_w - PAD * 2) / disk_cols;  // ~238
    const int32_t col2_x    = cell_w + PAD * 2;                  // second disk column x

    const int32_t res_h = sec_hdr_h + cpu_row_h + disk_rows * cell_h + PAD;

    lv_obj_t *res_card = lv_obj_create(s_page_thinklab);
    lv_obj_remove_style_all(res_card);
    lv_obj_add_style(res_card, &g_sty_card, 0);
    lv_obj_set_size(res_card, inner_w, res_h);
    lv_obj_set_pos(res_card, 0, 0);
    lv_obj_clear_flag(res_card, LV_OBJ_FLAG_SCROLLABLE);

    {
        // Section title — 36pt
        lv_obj_t *h = lv_label_create(res_card);
        lv_obj_set_style_text_font(h, &lv_font_montserrat_36, 0);
        lv_obj_set_style_text_color(h, lv_color_black(), 0);
        lv_label_set_text(h, "Resources");
        lv_obj_set_pos(h, 0, 0);

        // CPU — full-width row
        make_value_label(res_card, 0, sec_hdr_h, "CPU");
        s_lbl_tl_cpu = make_value_label(res_card, 160, sec_hdr_h, "--.- %");

        // Disk grid: 2 columns × 3 rows
        // Positions: [0] col0 row0, [1] col1 row0,
        //            [2] col0 row1, [3] col1 row1,
        //            [4] col0 row2
        const int32_t disk_start_y = sec_hdr_h + cpu_row_h;
        const int32_t col_x[2] = { 0, col2_x };

        for (int i = 0; i < 5; i++) {
            int32_t col = i % disk_cols;
            int32_t row = i / disk_cols;
            int32_t x   = col_x[col];
            int32_t y   = disk_start_y + row * cell_h;
            s_lbl_tl_disk[i] = make_disk_cell(res_card, x, y, cell_w, k_disk_labels[i]);
        }
    }

    // ── Services section ──────────────────────────────────────────────────────
    const int32_t svc_row_h = 70;
    const int32_t btn_w     = 130;
    const int32_t btn_h     = svc_row_h - 10;
    const int32_t svc_y     = res_h + GAP;
    const int32_t svc_h     = sec_hdr_h + 7 * svc_row_h + PAD;

    lv_obj_t *svc_card = lv_obj_create(s_page_thinklab);
    lv_obj_remove_style_all(svc_card);
    lv_obj_add_style(svc_card, &g_sty_card, 0);
    lv_obj_set_size(svc_card, inner_w, svc_h);
    lv_obj_set_pos(svc_card, 0, svc_y);
    lv_obj_clear_flag(svc_card, LV_OBJ_FLAG_SCROLLABLE);

    {
        // Section title — 36pt
        lv_obj_t *h = lv_label_create(svc_card);
        lv_obj_set_style_text_font(h, &lv_font_montserrat_36, 0);
        lv_obj_set_style_text_color(h, lv_color_black(), 0);
        lv_label_set_text(h, "Services");
        lv_obj_set_pos(h, 0, 0);

        const int32_t card_inner_w = inner_w - PAD * 2;
        const int32_t status_x     = card_inner_w - btn_w - 90;

        for (int i = 0; i < 7; i++) {
            int32_t y = sec_hdr_h + i * svc_row_h;
            int32_t vc = y + (svc_row_h - 34) / 2;  // vertical centre for 28pt text

            // Service name
            lv_obj_t *lbl_name = lv_label_create(svc_card);
            lv_obj_add_style(lbl_name, &g_sty_lbl_lg, 0);
            lv_label_set_text(lbl_name, s_tl_svc[i].name);
            lv_obj_set_pos(lbl_name, 0, vc);

            // Status label
            s_tl_svc[i].lbl_status = lv_label_create(svc_card);
            lv_obj_add_style(s_tl_svc[i].lbl_status, &g_sty_lbl_lg, 0);
            lv_label_set_text(s_tl_svc[i].lbl_status, "--");
            lv_obj_set_pos(s_tl_svc[i].lbl_status, status_x, vc);

            // Toggle button — large tap target
            lv_obj_t *btn = lv_btn_create(svc_card);
            lv_obj_remove_style_all(btn);
            lv_obj_add_style(btn, &g_sty_btn, 0);
            lv_obj_set_size(btn, btn_w, btn_h);
            lv_obj_set_pos(btn, card_inner_w - btn_w, y + 5);
            lv_obj_add_event_cb(btn, cb_tl_svc, LV_EVENT_CLICKED, &s_tl_svc[i]);
            s_tl_svc[i].btn = btn;

            lv_obj_t *l = lv_label_create(btn);
            lv_obj_add_style(l, &g_sty_lbl_lg, 0);
            lv_label_set_text(l, "Toggle");
            lv_obj_center(l);

            apply_svc_style(&s_tl_svc[i]);
        }
    }
}

lv_obj_t *get_page_thinklab() { return s_page_thinklab; }

// ── Update functions ──────────────────────────────────────────────────────────
bool thinklab_page_update_sensor(const char *topic, const char *value) {
    char buf[16];

    if (strcmp(topic, TOPIC_TL_CPU) == 0 && s_lbl_tl_cpu) {
        snprintf(buf, sizeof(buf), "%s %%", value);
        lv_label_set_text(s_lbl_tl_cpu, buf);
        return true;
    }

    const char *disk_topics[5] = {
        TOPIC_TL_DISK1, TOPIC_TL_DISK2, TOPIC_TL_DISK3, TOPIC_TL_DISK4, TOPIC_TL_DISK5
    };
    for (int i = 0; i < 5; i++) {
        if (strcmp(topic, disk_topics[i]) == 0 && s_lbl_tl_disk[i]) {
            snprintf(buf, sizeof(buf), "%s %%", value);
            lv_label_set_text(s_lbl_tl_disk[i], buf);
            Serial.printf("[ThinkLab] disk[%d] = %s\n", i, value);
            return true;
        }
    }
    return false;
}

bool thinklab_page_update_switch(const char *topic, bool on) {
    for (int i = 0; i < 7; i++) {
        if (strcmp(topic, s_tl_svc[i].topic_state) == 0) {
            s_tl_svc[i].on = on;
            apply_svc_style(&s_tl_svc[i]);
            return true;
        }
    }
    return false;
}
