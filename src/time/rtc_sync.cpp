/**
 * rtc_sync.cpp — NTP time sync for M5Stack PaperS3
 *
 * The PaperS3 has no external RTC chip accessible via M5.Rtc, so we rely
 * entirely on the ESP32's SNTP daemon (configTzTime) to keep the system
 * clock accurate.  Time is lost on power-off but resyncs within seconds
 * of the next WiFi connection.
 */

#include "rtc_sync.h"
#include "../config.h"
#include "../display/ui.h"

#include <Arduino.h>
#include <M5Unified.h>
#include <time.h>
#include <esp_sntp.h>

void rtc_sync_ntp() {
    configTzTime(TIMEZONE, NTP_SERVER);

    // Wait for SNTP to actually complete — getLocalTime() returns immediately
    // if the hardware RTC has any previously stored time, so we must wait for
    // the SNTP daemon to confirm a fresh sync.
    Serial.print("[RTC] waiting for NTP sync");
    const uint32_t deadline = millis() + 10000;
    while (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED) {
        delay(200);
        Serial.print('.');
        if (millis() > deadline) {
            Serial.println(" timeout");
            return;
        }
    }

    // Write synced time back to the hardware RTC so it survives power-off
    struct tm timeinfo;
    getLocalTime(&timeinfo, 0);
    M5.Rtc.setDateTime(&timeinfo);

    Serial.printf(" ok — %04d-%02d-%02d %02d:%02d:%02d\n",
                  timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                  timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

void rtc_restore_from_hw() {
    auto dt = M5.Rtc.getDateTime();
    if (dt.date.year < 2024) return;  // RTC not set yet

    // Set the ESP32 system clock from the hardware RTC so the date is correct
    // immediately at boot, before NTP sync.
    struct tm t = {};
    t.tm_year  = dt.date.year - 1900;
    t.tm_mon   = dt.date.month - 1;
    t.tm_mday  = dt.date.date;
    t.tm_hour  = dt.time.hours;
    t.tm_min   = dt.time.minutes;
    t.tm_sec   = dt.time.seconds;
    t.tm_isdst = -1;
    time_t epoch = mktime(&t);
    struct timeval tv = { .tv_sec = epoch, .tv_usec = 0 };
    settimeofday(&tv, nullptr);
    setenv("TZ", TIMEZONE, 1);
    tzset();

    Serial.printf("[RTC] restored from HW — %04d-%02d-%02d\n",
                  dt.date.year, dt.date.month, dt.date.date);
}

void rtc_update_ui() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo, 0)) return;   // not synced yet
    if (timeinfo.tm_year + 1900 < 2024) return;  // sanity check

    char month[16];
    strftime(month, sizeof(month), "%B", &timeinfo);
    char buf[24];
    snprintf(buf, sizeof(buf), "%d. %s", timeinfo.tm_mday, month);
    ui_update_date(buf);
}
