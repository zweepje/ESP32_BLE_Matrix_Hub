#pragma once
#include "web/Endpoint.h"
#include "web/Helpers.h"
#include "web/device/Helpers.h"

Endpoint device_raw_setBrightness("/device/raw/setBrightness", HTTP_GET, [](AsyncWebServerRequest* request) {
    iPixelDevice* device = getParamIPixelDevice(request, "device");
    long brightness = getParamLong(request, "brightness");
    device->setBrightness(brightness);
    request->send(200, "text/plain", "OK");
});