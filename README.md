# PaperS3 Dashboard

A touch-enabled home automation dashboard for the **M5Stack PaperS3** (SKU C139) e-paper display, built with LVGL and integrated with Home Assistant via MQTT.

![M5Stack PaperS3](https://static-cdn.m5stack.com/resource/docs/products/core/M5Paper_S3/img-m5papers3-01.webp)

---

## Features

- 540×960 e-paper display driven by [epdiy](https://github.com/vroland/epdiy) via M5Unified
- Touch navigation (GT911) across 8 dashboard pages
- Full MQTT integration with Home Assistant via `mqtt_statestream`
- Browser-based OTA firmware updates (no cable needed)
- Light sleep with touch wake — battery friendly
- LVGL 8.3 UI allocated entirely in 8 MB PSRAM

---

## Pages

| Page | Description |
|---|---|
| **Home** | Grid launcher for all pages |
| **Heating** | Per-room radiator control — temperature, preset, window state |
| **Switches** | TV, Shield, living/kitchen lights, sockets |
| **Cleaning** | Roborock S6 MaxV — room selection, repeat, start/pause/dock |
| **Climate** | Outdoor temp/humidity, CO₂, PM2.5 |
| **ThinkLab** | Proxmox server — CPU, disk usage, LXC/VM start/stop |
| **Arthur** | Child's room controls |
| **Bedroom** | Bedroom RGB strip and bulb |

---

## Hardware

- **M5Stack PaperS3** (ESP32-S3, 16 MB flash, 8 MB PSRAM, 540×960 e-paper)
- Power via USB-C or internal LiPo battery

---

## Software Stack

| Component | Version |
|---|---|
| PlatformIO / Arduino | espressif32 |
| M5Unified | ^0.2.5 |
| LVGL | ^8.3.0 |
| PubSubClient | ^2.8.0 |
| ArduinoJson | ^7.0.0 |

---

## Setup

### 1. Clone and configure credentials

```bash
git clone https://github.com/Boisti13/papers3-dashboard.git
cd papers3-dashboard
cp src/secrets.h.template src/secrets.h
```

Edit `src/secrets.h` and fill in your WiFi and MQTT broker credentials:

```cpp
#define SECRET_WIFI_SSID     "your-ssid"
#define SECRET_WIFI_PASSWORD "your-password"

#define SECRET_MQTT_HOST "192.168.x.x"
#define SECRET_MQTT_USER "mqtt-user"
#define SECRET_MQTT_PASS "mqtt-password"
```

> `src/secrets.h` is gitignored and will never be committed.

### 2. Adjust topic names

All MQTT topic mappings live in [src/config.h](src/config.h). The topics follow Home Assistant's `mqtt_statestream` naming convention (`homeassistant/<domain>/<entity_id>/state`). Update the entity IDs to match your own HA instance.

### 3. Build and flash

Open the project in PlatformIO (VS Code extension or CLI) and upload:

```bash
pio run --target upload
```

---

## Home Assistant Integration

The dashboard uses two mechanisms to communicate with Home Assistant:

### State: `mqtt_statestream`

HA pushes entity state changes to MQTT automatically. Add this to your `configuration.yaml`:

```yaml
mqtt_statestream:
  base_topic: homeassistant
  publish_attributes: true
  include:
    entities:
      - sensor.bathroom_temperature
      - sensor.bathroom_humidity
      - climate.bathroom_radiator
      - binary_sensor.bathroom_window_contact
      - sensor.kitchen_temperature
      - sensor.kitchen_humidity
      - climate.kitchen_radiator
      - binary_sensor.kitchen_window_contact
      - sensor.bedroom_temperature
      - sensor.bedroom_humidity
      - climate.bedroom_radiator
      - binary_sensor.bedroom_window_contact
      - sensor.living_room_temperature
      - sensor.living_room_humidity
      - climate.living_room_radiator
      - binary_sensor.balcony_door_contact
      - sensor.childs_room_temperature
      - sensor.childs_room_humidity
      - climate.arthur_radiator
      - binary_sensor.arthur_window_contact
      - remote.samsung_7_series_50
      - switch.tv_thinklab_l2
      - remote.shield
      - light.living_led
      - light.kitchen_main_led
      - switch.arthur_socket
      - input_boolean.vac_room_bad
      - input_boolean.vac_room_flur
      - input_boolean.vac_room_schlafzimmer
      - input_boolean.vac_room_wohnzimmer
      - input_boolean.vac_room_kinderzimmer
      - input_boolean.vac_room_kuche
      - input_select.vacuum_repeat
      - sensor.outdoor_2_temperature
      - sensor.outdoor_2_humidity
      - sensor.indoor_carbon_dioxide
      - sensor.airquality_1_pm25
      - sensor.thinklab_cpu_usage
      - sensor.thinklab_mount_mntssd256gb25
      - sensor.thinklab_mount_mntdisk500gb25
      - sensor.thinklab_mount_mntdisk5tb25
      - sensor.thinklab_mount_mntdisk2tb35
      - sensor.thinklab_mount_mntdisk8tb35
      - switch.lxc_audiobookshelf_102
      - switch.lxc_jellyfin_101
      - switch.lxc_pihole_103
      - switch.lxc_smb_share_106
      - switch.lxc_zerotier_one_105
      - switch.lxc_sprout_track_108
      - switch.vm_haos16_1_100
      - light.bedroom_rgb_strip
      - light.bedroom_rgb_bulb
```

### Commands: HA Package

The dashboard publishes MQTT commands when the user interacts with the UI (e.g. toggling a switch, adjusting a thermostat). HA needs automations to act on these.

Install the included HA package by copying [ha/papers3dash.yaml](ha/papers3dash.yaml) to your HA config:

```
config/packages/papers3dash.yaml
```

Then reference it in `configuration.yaml`:

```yaml
homeassistant:
  packages:
    papers3dash: !include packages/papers3dash.yaml
```

The package includes automations for:
- Climate set temperature / preset mode
- Switch, light, and remote on/off
- Roborock vacuum — room selection, start, pause, dock
- Disk sensor push on device reconnect (since statestream only publishes on change)

---

## OTA Updates

The dashboard includes a browser-based OTA server. From the Settings page on the device, activate OTA mode. Then navigate to the device's IP address in a browser, upload a `.bin` file built with PlatformIO, and the device reboots automatically.

A helper script is also included:

```bash
./ota_upload.sh <device-ip> <firmware.bin>
```

---

## Device Telemetry

The dashboard publishes its own state to MQTT for monitoring in HA:

| Topic | Content |
|---|---|
| `papers3dash/availability` | `online` / `offline` |
| `papers3dash/sensor/battery/state` | Battery percentage |
| `papers3dash/sensor/wifi_rssi/state` | WiFi signal strength (dBm) |

---

## Project Structure

```
src/
├── main.cpp
├── config.h              # All MQTT topics and pin definitions
├── secrets.h             # Gitignored — credentials
├── secrets.h.template    # Copy and fill in
├── lv_conf.h             # LVGL config (PSRAM-backed allocator)
├── display/
│   ├── epd_driver.*      # epdiy full/fast refresh wrapper
│   ├── nav.*             # Page navigation
│   ├── styles.*          # Global LVGL styles
│   ├── ui.*              # LVGL init, header, status bar
│   └── pages/            # One subfolder per page
├── fonts/                # MDI icon font for LVGL
├── mqtt/                 # PubSubClient wrapper + topic dispatch
├── ota/                  # Browser OTA server
├── power/                # Battery ADC, light sleep
├── time/                 # NTP sync via RTC
└── touch/                # GT911 driver wrapper
ha/
└── papers3dash.yaml      # Home Assistant package
```

---

## License

MIT — see [LICENSE](LICENSE).
