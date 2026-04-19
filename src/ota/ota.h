#pragma once

/**
 * ota.h — Browser-based OTA update server.
 *
 * On activation, starts an HTTP server on port 80.
 * User navigates to http://<IP>/ and uploads a .bin firmware file.
 * The ESP reboots automatically on successful upload.
 *
 * While active, light sleep is inhibited via sleep_inhibit(true).
 */

/** Start the OTA web server. Returns false if WiFi is not connected. */
bool ota_activate();

/** Stop the OTA web server and re-enable sleep. */
void ota_deactivate();

/** Call every loop iteration — handles incoming HTTP clients. */
void ota_loop();

/** Returns true while the OTA server is running. */
bool ota_is_active();

/** Returns the device IP string (e.g. "192.168.1.100"), or "" if inactive. */
const char *ota_get_ip();
