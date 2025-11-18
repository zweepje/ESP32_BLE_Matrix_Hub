#pragma once
#include "web/Endpoint.h"
#include "web/Helpers.h"
#include "web/device/Helpers.h"

Endpoint device_raw_setTime("/device/raw/setTime", HTTP_GET, [](AsyncWebServerRequest* request) {
    iPixelDevice* device = getParamIPixelDevice(request, "device");
    long hour = getParamLong(request, "hour");
    long minute = getParamLong(request, "minute");
    long second = getParamLong(request, "second");
    device->setTime(
        hour,
        minute,
        second
    );
    request->send(200, "text/plain", "OK");
});