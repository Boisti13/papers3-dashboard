# Changelog

All notable changes, newest first. Versions follow [semantic versioning](https://semver.org/); each release is tagged `vX.Y.Z` on `main`.

## Unreleased

Merged to `main` on 2026-05-22 (pull request #4) but not tagged yet; the next release will be v1.4.0.

### Added
- **Battery icon in the header** (Material Design Icons): level in steps, charging, and *USB, full*.
- **Battery card on the Settings page**: voltage, percentage and USB state.

### Changed
- **Snappier e-paper**: a touch redraws the screen at once instead of waiting for the next refresh slot, page changes use the fast mode, and every fifth refresh is a full-quality pass that clears ghosting.
- The Wi-Fi card on the Settings page is shorter; the OTA and sleep cards are spaced evenly.

### Fixed
- **Date lost after power-off**: the time is now restored from the hardware RTC at boot, and NTP writes back to it correctly.
- **Battery and USB detection** were unreliable when read from GPIOs; they now come from M5Unified's power API.
- **Grey boxes on the screen** after partial updates: LVGL now draws into a full-screen buffer (in PSRAM), so there are no band updates.

### Under the hood
- Documentation brought in line with the other projects: README rewritten (badges, highlights, install steps, entity tables, layout), this changelog, a `CLAUDE.md` with the working rules. The Home Assistant package's header comment no longer says `mqtt_statestream` must go into `configuration.yaml` — it works as a package.
- `.claude/settings.local.json` (one PC's local Claude Code permissions) is no longer tracked.

## v1.3.0 — 2026-04-20

### Changed
- **Switches page** has a 2×4 grid instead of 2×3; the two new tiles are free for future switches. Tile height follows `SW_ROWS`.

## v1.2.0 — 2026-04-19

### Added
- **Wi-Fi and MQTT card on the Settings page**: status, SSID, signal in dBm with a quality label (*Excellent*, *Good*, *Weak*, *Poor*), IP address, MQTT connected or not.

### Changed
- Before going to sleep, the dashboard goes back to the home page if it was on Settings or ThinkLab, so the standby picture isn't a page of stale numbers.
- The OTA button sits at the bottom of its card with consistent spacing.

## v1.1.0 — 2026-04-19

### Added
- **Wi-Fi signal in the header**: four bars (Material Design Icons) from the RSSI, and a Wi-Fi-off icon when disconnected.

### Changed
- **Non-blocking boot**: the home page appears immediately; Wi-Fi connects in the background.
- **Wi-Fi reconnects on its own**, with a forced reconnect if it hasn't come back after 30 s; NTP syncs again on each reconnect.

## v1.0.0 — 2026-04-19

### Added
- First version: LVGL 8.3 dashboard for the M5Stack PaperS3 with eight pages (Heating, Switches, Cleaning, Climate, ThinkLab, Arthur, Bedroom, Settings) and a home launcher.
- Home Assistant over MQTT: values from `mqtt_statestream`, commands handled by the package `ha/papers3dash.yaml`; battery and Wi-Fi signal as MQTT-discovery sensors.
- GT911 touch, light sleep with touch wake and power-off after longer inactivity (times set on the Settings page, kept in NVS), browser OTA updates and `ota_upload.sh`.
- README with setup, Home Assistant integration and OTA.
