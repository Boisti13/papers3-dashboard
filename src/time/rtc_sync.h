#pragma once

/**
 * Restore system clock from the hardware RTC at boot.
 * Call once in setup() after M5.begin() — gives correct date immediately
 * without waiting for WiFi or NTP.
 */
void rtc_restore_from_hw();

/**
 * Sync ESP32 SNTP from NTP, then write the result back to the hardware RTC.
 * Call once after WiFi is confirmed connected.
 * Blocks up to ~10 s waiting for the first SNTP response.
 */
void rtc_sync_ntp();

/**
 * Read the current date from the RTC and push it to the UI header.
 * Call periodically from loop() (e.g. every 60 s).
 */
void rtc_update_ui();
