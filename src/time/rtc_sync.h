#pragma once

/**
 * Sync ESP32 SNTP from NTP, then write the result into M5Unified's RTC.
 * Call once after WiFi is confirmed connected.
 * Blocks up to ~10 s waiting for the first SNTP response.
 */
void rtc_sync_ntp();

/**
 * Read the current date from the RTC and push it to the UI header.
 * Call periodically from loop() (e.g. every 60 s).
 */
void rtc_update_ui();
