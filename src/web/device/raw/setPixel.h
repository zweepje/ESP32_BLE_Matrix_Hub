#pragma once
#include "web/Endpoint.h"
#include "web/Helpers.h"
#include "web/device/Helpers.h"

Endpoint device_raw_setPixel("/device/raw/setPixel", HTTP_GET, [](AsyncWebServerRequest* request) {
    iPixelDevice* device = getParamIPixelDevice(request, "device");
    long x = getParamLong(request, "x");
    long y = getParamLong(request, "y");
    long r = getParamLong(request, "r");
    long g = getParamLong(request, "g");
    long b = getParamLong(request, "b");
    device->setPixel(
        x,
        y,
        r,
        g,
        b
    );
    request->send(200, "text/plain", "OK");
});