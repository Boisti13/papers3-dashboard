// Renders every dashboard page with sample values to 540x960 PNGs in the
// panel's 16 grey levels. Host build of the real UI code; see build.sh.
#include <lvgl.h>
#include <zlib.h>
#include "config.h"
#include "display/ui.h"
#include "display/nav.h"
#include "display/pages/home/home_page.h"
#include "display/pages/heating/heating_page.h"
#include "display/pages/switches/switches_page.h"
#include "display/pages/cleaning/cleaning_page.h"
#include "display/pages/climate/climate_page.h"
#include "display/pages/thinklab/thinklab_page.h"
#include "display/pages/arthur/arthur_page.h"
#include "display/pages/bedroom/bedroom_page.h"
#include "display/pages/settings/settings_page.h"
#include <stdio.h>
#include <string>
#include <vector>

static uint8_t g_fb[EPD_WIDTH * EPD_HEIGHT];

static void flush_cb(lv_disp_drv_t *drv, const lv_area_t *a, lv_color_t *px) {
    for (int y = a->y1; y <= a->y2; y++)
        for (int x = a->x1; x <= a->x2; x++) {
            uint8_t b = lv_color_brightness(*px++);
            g_fb[y * EPD_WIDTH + x] = (uint8_t)(((b + 8) / 17) * 17);  // 16 greys
        }
    lv_disp_flush_ready(drv);
}

static void put32(std::vector<uint8_t> &v, uint32_t x) {
    for (int i = 3; i >= 0; i--) v.push_back((x >> (i * 8)) & 0xff);
}
static void chunk(FILE *f, const char *type, const std::vector<uint8_t> &data) {
    std::vector<uint8_t> c; put32(c, data.size());
    c.insert(c.end(), type, type + 4); c.insert(c.end(), data.begin(), data.end());
    uint32_t crc = crc32(0, c.data() + 4, c.size() - 4);
    put32(c, crc); fwrite(c.data(), 1, c.size(), f);
}
static void write_png(const char *path) {
    std::vector<uint8_t> raw;
    for (int y = 0; y < EPD_HEIGHT; y++) {
        raw.push_back(0);
        raw.insert(raw.end(), g_fb + y * EPD_WIDTH, g_fb + (y + 1) * EPD_WIDTH);
    }
    uLongf zl = compressBound(raw.size());
    std::vector<uint8_t> z(zl);
    compress2(z.data(), &zl, raw.data(), raw.size(), 9); z.resize(zl);
    FILE *f = fopen(path, "wb");
    static const uint8_t sig[8] = {137, 80, 78, 71, 13, 10, 26, 10};
    fwrite(sig, 1, 8, f);
    std::vector<uint8_t> ihdr; put32(ihdr, EPD_WIDTH); put32(ihdr, EPD_HEIGHT);
    ihdr.insert(ihdr.end(), {8, 0, 0, 0, 0});  // 8-bit greyscale
    chunk(f, "IHDR", ihdr); chunk(f, "IDAT", z); chunk(f, "IEND", {});
    fclose(f);
    printf("wrote %s\n", path);
}

static void sw(const char *t, bool on) { ui_update_switch(t, on); }
static void val(const char *t, const char *v) { ui_update_sensor(t, v); }

static void sample_values() {
    ui_update_date("16. October");
    ui_update_wifi(true, -58);
    ui_update_battery(4.02f, 78, false, false);
    settings_page_update_wifi(true, "HomeWiFi", -58, "192.168.1.60", true);
    settings_page_update_battery(4.02f, 78, false, false);

    struct Room { const char *t, *h, *tg, *act, *pre, *win, *mode;
                  const char *vt, *vh, *vtg, *vact, *vpre; bool open; const char *vmode; };
    Room rooms[] = {
        {TOPIC_HEAT_BATH_TEMP, TOPIC_HEAT_BATH_HUM, TOPIC_HEAT_BATH_TARGET, TOPIC_HEAT_BATH_ACTION,
         TOPIC_HEAT_BATH_PRESET, TOPIC_HEAT_BATH_WINDOW, TOPIC_HEAT_BATH_MODE,
         "20.8", "64", "22.0", "heating", "comfort", false, "heat"},
        {TOPIC_HEAT_KTCH_TEMP, TOPIC_HEAT_KTCH_HUM, TOPIC_HEAT_KTCH_TARGET, TOPIC_HEAT_KTCH_ACTION,
         TOPIC_HEAT_KTCH_PRESET, TOPIC_HEAT_KTCH_WINDOW, TOPIC_HEAT_KTCH_MODE,
         "20.1", "52", "19.0", "idle", "eco", true, "heat"},
        {TOPIC_HEAT_BED_TEMP, TOPIC_HEAT_BED_HUM, TOPIC_HEAT_BED_TARGET, TOPIC_HEAT_BED_ACTION,
         TOPIC_HEAT_BED_PRESET, TOPIC_HEAT_BED_WINDOW, TOPIC_HEAT_BED_MODE,
         "18.4", "55", "18.0", "idle", "eco", false, "heat"},
        {TOPIC_HEAT_LIVE_TEMP, TOPIC_HEAT_LIVE_HUM, TOPIC_HEAT_LIVE_TARGET, TOPIC_HEAT_LIVE_ACTION,
         TOPIC_HEAT_LIVE_PRESET, TOPIC_HEAT_LIVE_WINDOW, TOPIC_HEAT_LIVE_MODE,
         "21.2", "48", "21.5", "heating", "comfort", false, "heat"},
        {TOPIC_HEAT_ART_TEMP, TOPIC_HEAT_ART_HUM, TOPIC_HEAT_ART_TARGET, TOPIC_HEAT_ART_ACTION,
         TOPIC_HEAT_ART_PRESET, TOPIC_HEAT_ART_WINDOW, TOPIC_HEAT_ART_MODE,
         "20.6", "51", "20.5", "idle", "comfort", false, "heat"},
    };
    for (auto &r : rooms) {
        val(r.t, r.vt); val(r.h, r.vh); val(r.tg, r.vtg); val(r.act, r.vact);
        val(r.pre, r.vpre); val(r.mode, r.vmode); val(r.win, r.open ? "on" : "off");
    }

    sw(TOPIC_SW_TV_STATE, true);      sw(TOPIC_SW_TVSOCK_STATE, true);
    sw(TOPIC_SW_SHIELD_STATE, true);  sw(TOPIC_SW_LIVING_STATE, true);
    sw(TOPIC_SW_KITCHEN_STATE, false); sw(TOPIC_SW_HEATLAMP_STATE, false);
    sw(TOPIC_BED_NIGHTSTAND_STATE, true); sw(TOPIC_BED_LIGHT_STATE, false);

    sw(TOPIC_VAC_ROOM_BATH, false); sw(TOPIC_VAC_ROOM_HALL, true);
    sw(TOPIC_VAC_ROOM_BED, false);  sw(TOPIC_VAC_ROOM_LIVE, true);
    sw(TOPIC_VAC_ROOM_KIDS, false); sw(TOPIC_VAC_ROOM_KTCH, true);
    val(TOPIC_VAC_REPEAT, "1");

    val(TOPIC_OUT_TEMP, "11.4"); val(TOPIC_OUT_HUM, "78");
    val(TOPIC_CO2, "642");       val(TOPIC_PM25, "6");

    val(TOPIC_TL_CPU, "7.5");
    val(TOPIC_TL_DISK1, "41.2"); val(TOPIC_TL_DISK2, "63.0"); val(TOPIC_TL_DISK3, "72.8");
    val(TOPIC_TL_DISK4, "35.5"); val(TOPIC_TL_DISK5, "58.1");
    sw(TOPIC_TL_LXC_ABS, true); sw(TOPIC_TL_LXC_JEL, true); sw(TOPIC_TL_LXC_PIH, true);
    sw(TOPIC_TL_LXC_SMB, true); sw(TOPIC_TL_LXC_ZT, true);  sw(TOPIC_TL_LXC_SPR, true);
    sw(TOPIC_TL_VM_HAOS, true);
}

int main(int argc, char **argv) {
    std::string out = argc > 1 ? argv[1] : ".";
    lv_init();
    static lv_color_t buf[EPD_WIDTH * EPD_HEIGHT];
    static lv_disp_draw_buf_t db;
    lv_disp_draw_buf_init(&db, buf, nullptr, EPD_WIDTH * EPD_HEIGHT);
    static lv_disp_drv_t drv;
    lv_disp_drv_init(&drv);
    drv.hor_res = EPD_WIDTH; drv.ver_res = EPD_HEIGHT;
    drv.flush_cb = flush_cb; drv.draw_buf = &db; drv.full_refresh = 1;
    lv_disp_drv_register(&drv);

    ui_init();
    sample_values();

    struct { lv_obj_t *(*get)(); const char *name, *file; } pages[] = {
        {get_page_home, "Home", "home"},           {get_page_heating, "Heating", "heating"},
        {get_page_switches, "Switches", "switches"}, {get_page_cleaning, "Cleaning", "cleaning"},
        {get_page_climate, "Climate", "climate"},  {get_page_thinklab, "ThinkLab", "thinklab"},
        {get_page_arthur, "Arthur", "arthur"},     {get_page_bedroom, "Bedroom", "bedroom"},
        {get_page_settings, "Settings", "settings"},
    };
    for (auto &p : pages) {
        navigate_home();
        if (p.get != get_page_home) navigate_to(p.get(), p.name);
        lv_obj_invalidate(lv_scr_act());
        lv_refr_now(nullptr);
        write_png((out + "/" + p.file + ".png").c_str());
    }

    // A room's detail page: tap the Living Room tile on the Heating page.
    navigate_home();
    navigate_to(get_page_heating(), "Heating");
    lv_event_send(lv_obj_get_child(get_page_heating(), 3), LV_EVENT_CLICKED, nullptr);
    lv_obj_invalidate(lv_scr_act());
    lv_refr_now(nullptr);
    write_png((out + "/heating-room.png").c_str());
    return 0;
}
