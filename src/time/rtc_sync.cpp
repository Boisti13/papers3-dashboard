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
#include <time.h>

void rtc_sync_ntp() {
    // Start the SNTP daemon and configure timezone.
    configTzTime(TIMEZONE, NTP_SERVER);

    Serial.print("[RTC] waiting for NTP");
    struct tm timeinfo;
    const uint32_t deadline = millis() + 10000;
    while (!getLocalTime(&timeinfo, 500)) {
        Serial.print('.');
        if (millis() > deadline) {
            Serial.println(" timeout");
            return;
        }
    }
    Serial.printf(" ok — %04d-%02d-%02d %02d:%02d:%02d\n",
                  timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                  timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

void rtc_update_ui() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo, 0)) return;   // not synced yet
    if (timeinfo.tm_year + 1900 < 2024) return;  // sanity check

    char buf[16];
    snprintf(buf, sizeof(buf), "%02d.%02d.%04d",
             timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
    ui_update_date(buf);
}
