#include "arthur_page.h"
#include "../../styles.h"
#include "../../nav.h"

static lv_obj_t *s_page_arthur = nullptr;

void build_page_arthur() {
    s_page_arthur = make_page();
    lv_obj_t *lbl = lv_label_create(s_page_arthur);
    lv_obj_add_style(lbl, &g_sty_lbl_md, 0);
    lv_label_set_text(lbl, "Arthur \xe2\x80\x94 coming soon");
    lv_obj_center(lbl);
}

lv_obj_t *get_page_arthur() { return s_page_arthur; }

bool arthur_page_update_sensor(const char *topic, const char *value) {
    (void)topic; (void)value;
    return false;
}

bool arthur_page_update_switch(const char *topic, bool on) {
    (void)topic; (void)on;
    return false;
}
