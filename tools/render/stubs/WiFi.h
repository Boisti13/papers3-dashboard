#pragma once
#include "Arduino.h"
struct WiFiStub { void reconnect() {} };
extern WiFiStub WiFi;
