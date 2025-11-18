#pragma once
#include "web/Endpoint.h"
#include "web/Helpers.h"
#include "web/device/Helpers.h"

Endpoint device_raw_setLED("/device/raw/setLED", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("setled called.");
    iPixelDevice* device = getParamIPixelDevice(request, "device");
    bool led = getParamBool(request, "led");
    device->setLED(led);
    Serial.println("setled semd.");
    request->send(200, "text/plain", "OK");
});