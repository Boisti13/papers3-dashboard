#pragma once

#include "secrets.h"   // gitignored — WiFi + MQTT credentials live there

// ─── WiFi ─────────────────────────────────────────────────────────────────────
#define WIFI_SSID     SECRET_WIFI_SSID
#define WIFI_PASSWORD SECRET_WIFI_PASSWORD

// ─── MQTT broker ──────────────────────────────────────────────────────────────
#define MQTT_HOST      SECRET_MQTT_HOST
#define MQTT_PORT      1883
#define MQTT_USER      SECRET_MQTT_USER
#define MQTT_PASS      SECRET_MQTT_PASS
#define MQTT_CLIENT_ID  "papers3-dash"
#define MQTT_DEVICE_ID  "papers3dash"

// Device-side state topics (published by this dashboard)
#define MQTT_AVAIL_TOPIC    MQTT_DEVICE_ID "/availability"
#define MQTT_BAT_STATE      MQTT_DEVICE_ID "/sensor/battery/state"
#define MQTT_WIFI_STATE     MQTT_DEVICE_ID "/sensor/wifi_rssi/state"

// ─── Topic conventions ────────────────────────────────────────────────────────
// All subscribe topics use HA MQTT Statestream format:
//   homeassistant/<domain>/<entity_id>           → state value
//   homeassistant/<domain>/<entity_id>/<attr>    → attribute value
// Command topics are custom automations in HA listening for these publishes.
//
// Required HA configuration:
//   mqtt_statestream:
//     base_topic: homeassistant
//     publish_attributes: true
//     include:
//       entities: [all entities listed below]

// ─── Heating rooms ────────────────────────────────────────────────────────────
// Bathroom
#define TOPIC_HEAT_BATH_TEMP    "homeassistant/sensor/bathroom_temperature/state"
#define TOPIC_HEAT_BATH_HUM     "homeassistant/sensor/bathroom_humidity/state"
#define TOPIC_HEAT_BATH_CURR    "homeassistant/climate/bathroom_radiator/current_temperature"
#define TOPIC_HEAT_BATH_TARGET  "homeassistant/climate/bathroom_radiator/temperature"
#define TOPIC_HEAT_BATH_ACTION  "homeassistant/climate/bathroom_radiator/hvac_action"
#define TOPIC_HEAT_BATH_PRESET  "homeassistant/climate/bathroom_radiator/preset_mode"
#define TOPIC_HEAT_BATH_WINDOW  "homeassistant/binary_sensor/bathroom_window_contact/state"
#define TOPIC_HEAT_BATH_SET     "homeassistant/climate/bathroom_radiator/set_temperature"
#define TOPIC_HEAT_BATH_MODE    "homeassistant/climate/bathroom_radiator/state"

// Kitchen
#define TOPIC_HEAT_KTCH_TEMP    "homeassistant/sensor/kitchen_temperature/state"
#define TOPIC_HEAT_KTCH_HUM     "homeassistant/sensor/kitchen_humidity/state"
#define TOPIC_HEAT_KTCH_CURR    "homeassistant/climate/kitchen_radiator/current_temperature"
#define TOPIC_HEAT_KTCH_TARGET  "homeassistant/climate/kitchen_radiator/temperature"
#define TOPIC_HEAT_KTCH_ACTION  "homeassistant/climate/kitchen_radiator/hvac_action"
#define TOPIC_HEAT_KTCH_PRESET  "homeassistant/climate/kitchen_radiator/preset_mode"
#define TOPIC_HEAT_KTCH_WINDOW  "homeassistant/binary_sensor/kitchen_window_contact/state"
#define TOPIC_HEAT_KTCH_SET     "homeassistant/climate/kitchen_radiator/set_temperature"
#define TOPIC_HEAT_KTCH_MODE    "homeassistant/climate/kitchen_radiator/state"

// Bedroom
#define TOPIC_HEAT_BED_TEMP     "homeassistant/sensor/bedroom_temperature/state"
#define TOPIC_HEAT_BED_HUM      "homeassistant/sensor/bedroom_humidity/state"
#define TOPIC_HEAT_BED_CURR     "homeassistant/climate/bedroom_radiator/current_temperature"
#define TOPIC_HEAT_BED_TARGET   "homeassistant/climate/bedroom_radiator/temperature"
#define TOPIC_HEAT_BED_ACTION   "homeassistant/climate/bedroom_radiator/hvac_action"
#define TOPIC_HEAT_BED_PRESET   "homeassistant/climate/bedroom_radiator/preset_mode"
#define TOPIC_HEAT_BED_WINDOW   "homeassistant/binary_sensor/bedroom_window_contact/state"
#define TOPIC_HEAT_BED_SET      "homeassistant/climate/bedroom_radiator/set_temperature"
#define TOPIC_HEAT_BED_MODE     "homeassistant/climate/bedroom_radiator/state"

// Living Room
#define TOPIC_HEAT_LIVE_TEMP    "homeassistant/sensor/living_room_temperature/state"
#define TOPIC_HEAT_LIVE_HUM     "homeassistant/sensor/living_room_humidity/state"
#define TOPIC_HEAT_LIVE_CURR    "homeassistant/climate/living_room_radiator/current_temperature"
#define TOPIC_HEAT_LIVE_TARGET  "homeassistant/climate/living_room_radiator/temperature"
#define TOPIC_HEAT_LIVE_ACTION  "homeassistant/climate/living_room_radiator/hvac_action"
#define TOPIC_HEAT_LIVE_PRESET  "homeassistant/climate/living_room_radiator/preset_mode"
#define TOPIC_HEAT_LIVE_WINDOW  "homeassistant/binary_sensor/balcony_door_contact/state"
#define TOPIC_HEAT_LIVE_SET     "homeassistant/climate/living_room_radiator/set_temperature"
#define TOPIC_HEAT_LIVE_MODE    "homeassistant/climate/living_room_radiator/state"

// Arthur (child's room)
#define TOPIC_HEAT_ART_TEMP     "homeassistant/sensor/childs_room_temperature/state"
#define TOPIC_HEAT_ART_HUM      "homeassistant/sensor/childs_room_humidity/state"
#define TOPIC_HEAT_ART_CURR     "homeassistant/climate/arthur_radiator/current_temperature"
#define TOPIC_HEAT_ART_TARGET   "homeassistant/climate/arthur_radiator/temperature"
#define TOPIC_HEAT_ART_ACTION   "homeassistant/climate/arthur_radiator/hvac_action"
#define TOPIC_HEAT_ART_PRESET   "homeassistant/climate/arthur_radiator/preset_mode"
#define TOPIC_HEAT_ART_WINDOW   "homeassistant/binary_sensor/arthur_window_contact/state"
#define TOPIC_HEAT_ART_SET      "homeassistant/climate/arthur_radiator/set_temperature"
#define TOPIC_HEAT_ART_MODE     "homeassistant/climate/arthur_radiator/state"

// ─── Switches / Lights ────────────────────────────────────────────────────────
#define TOPIC_SW_TV_STATE       "homeassistant/remote/samsung_7_series_50/state"
#define TOPIC_SW_TV_CMD         "homeassistant/remote/samsung_7_series_50/set"
#define TOPIC_SW_TVSOCK_STATE   "homeassistant/switch/tv_thinklab_l2/state"
#define TOPIC_SW_TVSOCK_CMD     "homeassistant/switch/tv_thinklab_l2/set"
#define TOPIC_SW_SHIELD_STATE   "homeassistant/remote/shield/state"
#define TOPIC_SW_SHIELD_CMD     "homeassistant/remote/shield/set"
#define TOPIC_SW_LIVING_STATE   "homeassistant/light/living_led/state"
#define TOPIC_SW_LIVING_CMD     "homeassistant/light/living_led/set"
#define TOPIC_SW_KITCHEN_STATE  "homeassistant/light/kitchen_main_led/state"
#define TOPIC_SW_KITCHEN_CMD    "homeassistant/light/kitchen_main_led/set"
#define TOPIC_SW_HEATLAMP_STATE "homeassistant/switch/arthur_socket/state"
#define TOPIC_SW_HEATLAMP_CMD   "homeassistant/switch/arthur_socket/set"

// ─── Vacuum / Cleaning ────────────────────────────────────────────────────────
#define TOPIC_VAC_STATE         "homeassistant/vacuum/roborock_s6_maxv"
#define TOPIC_VAC_CMD           "homeassistant/vacuum/roborock_s6_maxv/set"
#define TOPIC_VAC_ROOM_BATH     "homeassistant/input_boolean/vac_room_bad/state"
#define TOPIC_VAC_ROOM_HALL     "homeassistant/input_boolean/vac_room_flur/state"
#define TOPIC_VAC_ROOM_BED      "homeassistant/input_boolean/vac_room_schlafzimmer/state"
#define TOPIC_VAC_ROOM_LIVE     "homeassistant/input_boolean/vac_room_wohnzimmer/state"
#define TOPIC_VAC_ROOM_KIDS     "homeassistant/input_boolean/vac_room_kinderzimmer/state"
#define TOPIC_VAC_ROOM_KTCH     "homeassistant/input_boolean/vac_room_kuche/state"
#define TOPIC_VAC_REPEAT        "homeassistant/input_select/vacuum_repeat/state"
#define TOPIC_VAC_REPEAT_SET    "homeassistant/input_select/vacuum_repeat/set"
#define TOPIC_VAC_START_ALL     "homeassistant/vacuum/roborock_s6_maxv/start"
#define TOPIC_VAC_PAUSE         "homeassistant/vacuum/roborock_s6_maxv/pause"
#define TOPIC_VAC_DOCK          "homeassistant/vacuum/roborock_s6_maxv/dock"
#define TOPIC_VAC_START_SEL     "homeassistant/script/vacuum_clean_selected_rooms/set"
#define TOPIC_VAC_TRASH         "homeassistant/script/roborock_go_to_trashcan/set"

// ─── Climate overview ─────────────────────────────────────────────────────────
#define TOPIC_OUT_TEMP          "homeassistant/sensor/outdoor_2_temperature/state"
#define TOPIC_OUT_HUM           "homeassistant/sensor/outdoor_2_humidity/state"
#define TOPIC_CO2               "homeassistant/sensor/indoor_carbon_dioxide/state"
#define TOPIC_PM25              "homeassistant/sensor/airquality_1_pm25/state"

// ─── ThinkLab (server monitor) ────────────────────────────────────────────────
#define TOPIC_TL_CPU            "homeassistant/sensor/thinklab_cpu_usage/state"
#define TOPIC_TL_DISK1          "homeassistant/sensor/thinklab_mount_mntssd256gb25/state"
#define TOPIC_TL_DISK2          "homeassistant/sensor/thinklab_mount_mntdisk500gb25/state"
#define TOPIC_TL_DISK3          "homeassistant/sensor/thinklab_mount_mntdisk5tb25/state"
#define TOPIC_TL_DISK4          "homeassistant/sensor/thinklab_mount_mntdisk2tb35/state"
#define TOPIC_TL_DISK5          "homeassistant/sensor/thinklab_mount_mntdisk8tb35/state"
#define TOPIC_TL_LXC_ABS        "homeassistant/switch/lxc_audiobookshelf_102/state"
#define TOPIC_TL_LXC_JEL        "homeassistant/switch/lxc_jellyfin_101/state"
#define TOPIC_TL_LXC_PIH        "homeassistant/switch/lxc_pihole_103/state"
#define TOPIC_TL_LXC_SMB        "homeassistant/switch/lxc_smb_share_106/state"
#define TOPIC_TL_LXC_ZT         "homeassistant/switch/lxc_zerotier_one_105/state"
#define TOPIC_TL_LXC_SPR        "homeassistant/switch/lxc_sprout_track_108/state"
#define TOPIC_TL_VM_HAOS        "homeassistant/switch/vm_haos16_1_100/state"
#define TOPIC_TL_LXC_ABS_CMD    "homeassistant/switch/lxc_audiobookshelf_102/set"
#define TOPIC_TL_LXC_JEL_CMD    "homeassistant/switch/lxc_jellyfin_101/set"
#define TOPIC_TL_LXC_PIH_CMD    "homeassistant/switch/lxc_pihole_103/set"
#define TOPIC_TL_LXC_SMB_CMD    "homeassistant/switch/lxc_smb_share_106/set"
#define TOPIC_TL_LXC_ZT_CMD     "homeassistant/switch/lxc_zerotier_one_105/set"
#define TOPIC_TL_LXC_SPR_CMD    "homeassistant/switch/lxc_sprout_track_108/set"
#define TOPIC_TL_VM_HAOS_CMD    "homeassistant/switch/vm_haos16_1_100/set"

// ─── Bedroom lights ──────────────────────────────────────────────────────────
#define TOPIC_BED_NIGHTSTAND_STATE "homeassistant/light/bedroom_rgb_strip/state"
#define TOPIC_BED_NIGHTSTAND_CMD   "homeassistant/light/bedroom_rgb_strip/set"
#define TOPIC_BED_LIGHT_STATE      "homeassistant/light/bedroom_rgb_bulb/state"
#define TOPIC_BED_LIGHT_CMD        "homeassistant/light/bedroom_rgb_bulb/set"

// ─── Power management ─────────────────────────────────────────────────────────
#define PIN_PWR_HOLD    44
#define PIN_CHG_STATE   4
#define PIN_USB_DET     5
#define PIN_ADC_BATTERY 3

// ─── Time / NTP ───────────────────────────────────────────────────────────────
#define NTP_SERVER  "pool.ntp.org"
#define TIMEZONE    "CET-1CEST,M3.5.0,M10.5.0/3"

// ─── Display ──────────────────────────────────────────────────────────────────
#define EPD_WIDTH   540
#define EPD_HEIGHT  960
