#!/bin/bash
# OTA upload to PaperS3 dashboard
# Usage: ./ota_upload.sh <IP>
# Example: ./ota_upload.sh 192.168.178.164

IP=${1:-192.168.178.164}
BIN=".pio/build/PaperS3/firmware.bin"

echo "Building firmware..."
~/.platformio/penv/Scripts/pio run || exit 1

echo "Uploading to http://$IP/..."
curl -X POST "http://$IP/update" \
     -F "firmware=@$BIN" \
     --progress-bar
