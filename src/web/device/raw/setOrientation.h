#pragma once
#include "web/Endpoint.h"
#include "web/Helpers.h"
#include "web/device/Helpers.h"

Endpoint device_raw_setOrientation("/device/raw/setOrientation", HTTP_GET, [](AsyncWebServerRequest* request) {
    iPixelDevice* device = getParamIPixelDevice(request, "device");
    long orientation = getParamLong(request, "orientation");
    device->setOrientation(orientation);
    request->send(200, "text/plain", "OK");
});