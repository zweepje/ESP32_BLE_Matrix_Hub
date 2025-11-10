#pragma once
#include "web/Endpoint.h"
#include "web/Helpers.h"
#include "web/device/Helpers.h"

Endpoint device_raw_setLED("/device/raw/setLED", HTTP_GET, [](AsyncWebServerRequest* request) {
    iPixelDevice* device = getParamIPixelDevice(request, "device");
    bool led = getParamBool(request, "led");
    device->setLED(led);
    request->send(200, "text/plain", "OK");
});