#pragma once
#include "web/Endpoint.h"
#include "web/Helpers.h"
#include "web/device/Helpers.h"

Endpoint device_raw_setSpeed("/device/raw/setSpeed", HTTP_GET, [](AsyncWebServerRequest* request) {
    iPixelDevice* device = getParamIPixelDevice(request, "device");
    long speed = getParamLong(request, "speed");
    device->setSpeed(speed);
    request->send(200, "text/plain", "OK");
});