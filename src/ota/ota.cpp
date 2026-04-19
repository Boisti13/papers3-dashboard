#include "ota.h"
#include "../power/sleep.h"

#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>
#include <Arduino.h>

static WebServer *s_server = nullptr;
static bool       s_active = false;
static char       s_ip[20] = {};

static const char *s_html =
    "<!DOCTYPE html><html><head><title>PaperS3 OTA</title>"
    "<style>"
    "body{font-family:sans-serif;max-width:480px;margin:60px auto;padding:0 20px}"
    "h2{margin-bottom:32px}"
    "input[type=file]{display:block;margin:24px 0;font-size:16px}"
    "input[type=submit]{padding:12px 32px;font-size:16px;cursor:pointer}"
    "</style></head><body>"
    "<h2>PaperS3 Firmware Update</h2>"
    "<form method='POST' action='/update' enctype='multipart/form-data'>"
    "<input type='file' name='firmware' accept='.bin'>"
    "<input type='submit' value='Upload &amp; Flash'>"
    "</form></body></html>";

bool ota_activate() {
    if (s_active) return true;
    if (WiFi.status() != WL_CONNECTED) return false;

    strlcpy(s_ip, WiFi.localIP().toString().c_str(), sizeof(s_ip));
    sleep_inhibit(true);

    s_server = new WebServer(80);

    s_server->on("/", HTTP_GET, []() {
        s_server->send(200, "text/html", s_html);
    });

    // POST handler: send response then restart
    s_server->on("/update", HTTP_POST,
        []() {
            s_server->sendHeader("Connection", "close");
            bool ok = !Update.hasError();
            s_server->send(200, "text/plain",
                           ok ? "Upload OK — rebooting…" : "Upload FAILED");
            delay(1000);
            if (ok) ESP.restart();
        },
        []() {
            HTTPUpload &upload = s_server->upload();
            if (upload.status == UPLOAD_FILE_START) {
                Serial.printf("[OTA] start: %s\n", upload.filename.c_str());
                Update.begin(UPDATE_SIZE_UNKNOWN);
            } else if (upload.status == UPLOAD_FILE_WRITE) {
                Update.write(upload.buf, upload.currentSize);
            } else if (upload.status == UPLOAD_FILE_END) {
                Update.end(true);
                Serial.printf("[OTA] done: %u bytes\n", upload.totalSize);
            }
        }
    );

    s_server->begin();
    s_active = true;
    Serial.printf("[OTA] server at http://%s/\n", s_ip);
    return true;
}

void ota_deactivate() {
    if (!s_active) return;
    if (s_server) {
        delete s_server;
        s_server = nullptr;
    }
    s_active = false;
    s_ip[0]  = '\0';
    sleep_inhibit(false);
    Serial.println("[OTA] server stopped");
}

void ota_loop() {
    if (s_active && s_server) {
        s_server->handleClient();
    }
}

bool ota_is_active() { return s_active; }
const char *ota_get_ip() { return s_ip; }
